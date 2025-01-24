import bpy
import bpy_extras
import bmesh
import math
import gpu
import gpu_extras.batch
import copy
import mathutils
import json
import socket
import time
import threading
import copy
import struct

serv_address = ('127.0.0.1', 50001)
# ソケットを作成する
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

bl_info = {
    "name": "メッシュシンク",
    "author": "Ryuta Shimazaki",
    "version": (1, 0),
    "blender": (3, 3, 1),
    "location": "",
    "description": "メッシュシンク",
    "Warning" : "",
    "wiki_url" : "",
    "tracker_url" : "",
    "category": "Object",
}

#メッシュ送信クラス
class send_Mesh:

    verticesMax = 128 #一度のデータに格納する最大頂点数

    verticesNum = 0 #頂点数
    verticesOffset = 0#頂点オフセット
    isLastData = 0 #最後のデータか
    mainData = list() #データ本体

    oldData = list()#古いデータ

    #activeObject = bmesh()

    #格納されたデータを送信する
    def send(self):
        #先頭に頂点数を封入
        message = struct.pack('<Q',self.verticesNum)

        #頂点オフセットを封入
        message += struct.pack('<Q',self.verticesOffset)

        #最後のデータか
        message += struct.pack('<Q',self.isLastData)
        
        #本体追加
        message += self.mainData
            
        #送信
        sock.sendto(message, serv_address)

    #メッシュシンク有効オブジェクトを探して処理を行う
    #id : ノードの各ステージないでの通し番号
    def parse_scene_recursive_json(self,object,level,id):

        #見つけたら処理
        if object.name == "field":
            self.straged_active_object(object)
            return

        #直接の子供リストを走査
        if len(object.children) > 0:
            #子ノードリストを作成
            #子ノードへ進むs
            for child in object.children:
                self.parse_scene_recursive_json(child,level + 1,0)
                cid+=1

    def straged_active_object(self,object):
        #編集モードの変更を即座に反映
        object.update_from_editmode()
        #アクティブオブジェクト取得
        me = object.data

        #有効なbmeshとして登録
        self.activeObject = bmesh.new()
        self.activeObject.from_mesh(me)
        #self.activeObject =  activeMesh


    #頂点データを格納する
    def straged_vertex(self):
    
        activeMesh = self.activeObject

        #有効化されたuvを一個取ってくる
        uvLayer = activeMesh.loops.layers.uv.active
        

        id=0#累計数
        verticesCount=0#位置データ内数
        self.verticesOffset = 0
        for face in activeMesh.faces :
            #三角面の時
            if( len(face.loops) == 3):
                for loop in face.loops :
                    #座標、UV取得
                    vertex = loop.vert
                    uv = loop[uvLayer].uv

                    #座標
                    #データの先頭の時は型指定の為代入
                    if verticesCount==0 :
                        self.mainData = struct.pack('<f',vertex.co.x)
                    else :
                        self.mainData += struct.pack('<f',vertex.co.x)
                    self.mainData += struct.pack('<f',vertex.co.z)
                    self.mainData += struct.pack('<f',vertex.co.y)
                    self.mainData += struct.pack('<f',1.0)

                    #UV座標
                    self.mainData += struct.pack('<f',uv.x)
                    self.mainData += struct.pack('<f',-uv.y)

                    #法線
                    self.mainData += struct.pack('<f',face.normal.x)
                    self.mainData += struct.pack('<f',face.normal.z)
                    self.mainData += struct.pack('<f',face.normal.y)

                    id = id + 1
                    verticesCount = verticesCount + 1
            #四角以上の鋭角面
            else :
                #頂点数-2個の三角面として扱う
                for i in range(0,len(face.loops)-2):
                    #勝手に三角面作る
                    for j in range(0,3) :
                        loopIndex = i
                        if(j==0):
                            loopIndex = 0
                        vertex = face.loops[j+loopIndex].vert
                        uv = face.loops[j+loopIndex][uvLayer].uv

                        #座標
                        #データの先頭の時は型指定の為代入
                        if verticesCount==0 :
                            self.mainData = struct.pack('<f',vertex.co.x)
                        else :
                            self.mainData += struct.pack('<f',vertex.co.x)
                        self.mainData += struct.pack('<f',vertex.co.z)
                        self.mainData += struct.pack('<f',vertex.co.y)
                        self.mainData += struct.pack('<f',1.0)

                        #UV座標
                        self.mainData += struct.pack('<f',uv.x)
                        self.mainData += struct.pack('<f',-uv.y)

                        #法線
                        self.mainData += struct.pack('<f',face.normal.x)
                        self.mainData += struct.pack('<f',face.normal.z)
                        self.mainData += struct.pack('<f',face.normal.y)

                        id = id + 1
                        verticesCount = verticesCount + 1
            #送信処理
            if(verticesCount > self.verticesMax or id == len(activeMesh.faces)):
                #最後のデータか?
                self.isLastData = 0
                if(id == len(activeMesh.faces)):
                   self.isLastData = 1

                #頂点数を更新
                self.verticesNum = id
                self.send()
                #送信後にオフセット位置を今送った頂点の位置にする
                self.verticesOffset = self.verticesNum
                verticesCount = 0
                
    #メッシュシンク適用オブジェクトを探して取得する
    def serch_object(self):

        #シーン内の全オブジェクト走査してパック
        #シーン内の全オブジェクトについて
        id = 0
        for object in bpy.context.scene.objects:
            #親オブジェクトがあるものはスキップ 
            if object.parent:
                continue
            #シーン直下のオブジェクトをルートノードとし再帰関数で走査
            self.parse_scene_recursive_json(object,0,id)
            id+=1

    #データ変更チェック
    def send_changing_data(self):
        if self.activeObject != self.oldData :
            self.oldData = self.activeObject
            return True
        self.oldData = self.activeObject
        return False
        
#メッシュシンク用の型合わせ用jsonファイル出力(現在は不要)
class MYADDON_OT_export_meshsyncobject(bpy.types.Operator,bpy_extras.io_utils.ExportHelper):
    bl_idname = "myadddon.myaddon_ot_meshsyncobject"
    bl_label = "MeshSyncオブジェクト出力"
    bl_description = "MeshSyncオブジェクトをExportします"
    #出力するファイルの拡張子
    filename_ext = ".json"

    def execute(self,context):
        print("Exportします")
        #print(bpy.context.scene.objects)

        self.export_json()

        print("Exportしました")

        self.report({'INFO'},"Exportしました")

        return {'FINISHED'}
    
    def write_and_print(self,file,str):
        print(str)

        file.write(str)
        file.write('\n')

    def prase_scene_recursive(self,file,object,level):
        """シーン解析用再帰関数"""
        
        indent = ''
        for i in range(level):
            indent += "\t"

        #オブジェクト名書き込み
        self.write_and_print(file,indent + object.type)
        trans, rot, scale = object.matrix_local.decompose()
        #rot
        rot = rot.to_euler()
        rot.x = math.degrees(rot.x)
        rot.y = math.degrees(rot.y)
        rot.z = math.degrees(rot.z)
        #draw
        self.write_and_print(file,indent + "T %f %f %f" % (trans.x,trans.y,trans.z))
        self.write_and_print(file,indent + "R %f %f %f" % (rot.x,rot.y,rot.z))
        self.write_and_print(file,indent + "S %f %f %f" % (scale.x,scale.y,scale.z))
        #カスタムプロパティ'file_name'
        if "file_name" in object:
            self.write_and_print(file,indent + "N %s" % object["file_name"])
        #カスタムプロパティ'collider'
        if "collider" in object:
            self.write_and_print(file,indent + "C %s" % object["collider"])
            temp_str = indent + "CC %f %f %f"
            temp_str %= (object["collider_center"][0],object["collider_center"][1],object["collider_center"][2])
            self.write_and_print(file,temp_str)
            temp_str = indent + "CS %f %f %f"
            temp_str %= (object["collider_size"][0],object["collider_size"][1],object["collider_size"][2],)
            self.write_and_print(file,temp_str)
        self.write_and_print(file,indent + 'END')
        self.write_and_print(file,'')

        for child in object.children:
            self.prase_scene_recursive(file,child,level+1)
            
    def parse_scene_recursive_json(self,data_parent,object,level,id):

        #シーンのオブジェクト一個分のjsonオブジェクト作成
        json_object = dict()
        #一個分のjsonオブジェクトを親オブジェクトに登録
        #data_parent.append(json_object)

        if object.name == "field":
            self.export_field_vertex(object,data_parent)

        #直接の子供リストを走査
        if len(object.children) > 0:
            #子ノードリストを作成
            #子ノードへ進むs
            for child in object.children:
                self.parse_scene_recursive_json(data_parent,child,level + 1,0)
                cid+=1

    def export_field_vertex(self,object,json):
        json_object_f = dict()
        
        #編集モードの変更を即座に反映
        object.update_from_editmode()
        #アクティブオブジェクト取得
        me = object.data

        # Get a BMesh representation
        activeMesh = bmesh.new()   # create an empty BMesh
        activeMesh.from_mesh(me)


        id=0
        mesh = object.data
        uv_layer = activeMesh.loops.layers.uv.active
        
        for face in activeMesh.faces :
            for loop in face.loops :
                vertex = loop.vert
                uv = loop[uv_layer].uv

                json_object = dict() 
                json_object["i"] = id
                json_object["p"] = (float(format(vertex.co.x, '.2f')),float(format(vertex.co.y, '.2f')),float(format(vertex.co.z, '.2f')))
                json_object["u"] = (float(format(uv.x, '.3f')),float(format(uv.y, '.3f')))
                json_object["n"] = (float(format(face.normal.x, '.2f')),float(format(face.normal.y, '.2f')),float(format(face.normal.z, '.2f')))
                
                #まとめて一個分のjsonオブジェクトに登録
                json["v"].append(json_object)
                id = id + 1

    def export_json(self):
        """JSON形式で"""
        json_object = dict()
        json_object["m"] = dict()
        datas = json_object["m"]
        datas["v"] = list()

        #シーン内の全オブジェクト走査してパック
        #シーン内の全オブジェクトについて
        id = 0
        for object in bpy.context.scene.objects:
            #親オブジェクトがあるものはスキップ 
            if object.parent:
                continue
            #シーン直下のオブジェクトをルートノードとし再帰関数で走査
            self.parse_scene_recursive_json(datas,object,0,id)
            id+=1
        self.json_object_root = json_object

         #オブジェクトをJSON文字列にエンコード
        json_text = json.dumps(json_object,ensure_ascii=False,cls=json.JSONEncoder,indent=4)
        #コンソールに表示する
        print(json_text)

        #ファイルオープン
        with open(self.filepath,"wt",encoding="utf-8") as file:
            file.write(json_text)


class TOPBAR_MT_my_menu(bpy.types.Menu):
    #Blenderがクラスを識別するための固有の文字列
    bl_idname = "TOPBAR_MT_my_menu"
    #メニューのラベルとして表示される文字列
    bl_label = "MyMenu"

    bl_description = "拡張メニュー by " + bl_info["author"]

    #サブメニューの描画
    def draw(self,context):
        self.layout.operator(MYADDON_OT_export_meshsyncobject.bl_idname, text=MYADDON_OT_export_meshsyncobject.bl_label)
    
    #既存メニューにサブメニューを追加
    def submenu(self,context):
        self.layout.menu(TOPBAR_MT_my_menu.bl_idname)

class OBJECT_PT_file_name(bpy.types.Panel):
    """オブジェクトのファイルネームパネル"""
    bl_idname = "OBJECT_PT_file_name"
    bl_label = "FileName"
    bl_space_type = "PROPERTIES"
    bl_region_type = "WINDOW"
    bl_context = "object"
    
    def draw(self,context):
        if "file_name" in context.object:
            self.layout.prop(context.object,'["file_name"]',text=self.bl_label)
        else:
            self.layout.operator(MYADDON_OT_add_filename.bl_idname)

class MYADDON_OT_add_filename(bpy.types.Operator):
    bl_idname = "myaddon.myaddon_ot_add_filename"
    bl_label = "FileName 追加"
    bl_description = "['file_name']カスタムプロパティを追加します"
    bl_options = {"REGISTER","UNDO"}

    def execute(self,context):

        context.object["file_name"] = ""

        return {"FINISHED"}


#Blenderに登録するクラスリスト
classes = (
    MYADDON_OT_export_meshsyncobject,
    TOPBAR_MT_my_menu,
    MYADDON_OT_add_filename,
    OBJECT_PT_file_name,
)

isSend = 1
isSendFileld = 1

#送信用ループ処理
def mesh_sync_loop():
    sMesh = send_Mesh()
    #無効化されるまでずっと
    while isSend==1:
        sMesh.serch_object()
        #if sMesh.send_changing_data():
        sMesh.straged_vertex()
        
#スレッドまとめ
threads = []

#アドオン有効化時コールバックS
def register():
    #Blenderにクラスを登録
    for cls in classes:
        bpy.utils.register_class(cls)

    #メニューに項目追加
    bpy.types.TOPBAR_MT_editor_menus.append(TOPBAR_MT_my_menu.submenu)
    #3Dビューに描画関数追加
    print("メッシュシンク機能が有効化されました。")
    
    #グローバル変数を内部で使う
    global isSend
    isSend = 1

    #スレッド開始
    thread = threading.Thread(target=mesh_sync_loop)
    threads.append(thread)
    thread.start()
    #asyncio.run(send(jdata,sjson))

#アドオン無効化時コールバック
def unregister():
    #メニューから項目削除
    bpy.types.TOPBAR_MT_editor_menus.remove(TOPBAR_MT_my_menu.submenu)
    
    # Blenderからクラスを削除
    for cls in classes:
        bpy.utils.unregister_class(cls)
    
    print("レベルエディタが無効化されました。")
    
    #スレッド閉じる
    global isSend
    isSend = 0
    for t in threads:
        t.join()

if __name__ == "__main__":
    register()