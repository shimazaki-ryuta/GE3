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

serv_address = ('127.0.0.1', 50001)
# ソケットを作成する
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

bl_info = {
    "name": "レベルエディタ",
    "author": "Ryuta Shimazaki",
    "version": (1, 0),
    "blender": (3, 3, 1),
    "location": "",
    "description": "レベルエディタ",
    "Warning" : "",
    "wiki_url" : "",
    "tracker_url" : "",
    "category": "Object",
}

class MYADDON_OT_stretch_vertex(bpy.types.Operator):
    bl_idname = "myadddon.myaddon_ot_stretch_vertex"
    bl_label = "頂点を伸ばす"
    bl_description = "頂点座標を引っ張って伸ばします"
    bl_options = {'REGISTER','UNDO'}

    def execute(self,context):
        bpy.data.objects["Cube"].data.vertices[0].co.x += 1.0
        print("頂点を伸ばしました")

        return {'FINISHED'}
    
class MYADDON_OT_create_ico_sphere(bpy.types.Operator):
    bl_idname = "myadddon.myaddon_ot_create_ico_sphere"
    bl_label = "ICO球生成"
    bl_description = "ICO球を生成します"
    bl_options = {'REGISTER','UNDO'}

    def execute(self,context):
        bpy.ops.mesh.primitive_ico_sphere_add()
        print("ICO球を生成しました")

        return {'FINISHED'}

class SendJson:

    jData = "json"
    def Send(self):
    #def Send():

        message = self.jData + "\0"
        #message = "{""name"": ""scene""}"
        if message != 'end':
            sock.sendto(message.encode('utf-8'), serv_address)

    #id : ノードの各ステージないでの通し番号
    def parse_scene_recursive_json(self,data_parent,object,level,id):

        #シーンのオブジェクト一個分のjsonオブジェクト作成
        json_object = dict()
        json_object["id"] = id
        json_object["type"] = object.type
        json_object["name"] = object.name

        #その他情報パック
        trans, rot, scale = object.matrix_local.decompose()
        #rot
        rot = rot.to_euler()
        #rot.x = math.degrees(rot.x)
        #rot.y = math.degrees(rot.y)
        #rot.z = math.degrees(rot.z)
        #トランスフォーム情報をディクショナリに登録
        transform = dict()
        transform["translation"] = (trans.x,trans.y,trans.z)
        transform["rotation"] = (rot.x,rot.y,rot.z)
        transform["scaling"] = (scale.x,scale.y,scale.z)
        #まとめて一個分のjsonオブジェクトに登録
        json_object["transform"] = transform

        #deltaトランスフォーム
        dtrans = object.delta_location
        drot = object.delta_rotation_euler
        dscale = object.delta_scale
        #rot
        #drot = drot.to_euler()
        dtransform = dict()
        dtransform["translation"] = (dtrans.x,dtrans.y,dtrans.z)
        dtransform["rotation"] = (drot.x,drot.y,drot.z)
        dtransform["scaling"] = (dscale.x,dscale.y,dscale.z)
        #まとめて一個分のjsonオブジェクトに登録
        json_object["delta_transform"] = dtransform

        #カスタムプロパティ 'file_name'
        if "file_name" in object:
            json_object["file_name"] = object["file_name"]

        #カスタムプロパティ 'collider'
        if  "collider" in object:
            collider = dict()
            collider["type"] = object["collider"]
            collider["center"] = object["collider_center"].to_list()
            collider["size"] = object["collider_size"].to_list()
            json_object["collider"] = collider

        

        #一個分のjsonオブジェクトを親オブジェクトに登録
        data_parent.append(json_object)

        #直接の子供リストを走査
        if len(object.children) > 0:
            #子ノードリストを作成
            json_object["children"] = list()
            cid=0
            #子ノードへ進むs
            for child in object.children:
                self.parse_scene_recursive_json(json_object["children"],child,level + 1,cid)
                cid+=1

    def export_json(self):
        """JSON形式で"""
        #保存する情報をまとめる
        json_object_root = dict()

        #ノード名
        json_object_root["name"] = "scene"
        #オブジェクトリストを作成
        json_object_root["objects"] = list()

        #シーン内の全オブジェクト走査してパック
        #シーン内の全オブジェクトについて
        id = 0
        for object in bpy.context.scene.objects:
            #親オブジェクトがあるものはスキップ 
            if object.parent:
                continue
            #シーン直下のオブジェクトをルートノードとし再帰関数で走査
            self.parse_scene_recursive_json(json_object_root["objects"],object,0,id)

            id+=1

        #オブジェクトをJSON文字列にエンコード
        json_text = json.dumps(json_object_root,ensure_ascii=False,cls=json.JSONEncoder,indent=0)
        #コンソールに表示する
        #print(json_text)

        #classのデータにコピー
        self.jData = json_text

class SendField:
    jData = "json"
    json_object_root = dict()
    dumpJson = dict()
    def Send(self):
        message = self.jData + "\0"
        if message != 'end':
            #そうしん回数
            sendDatas = list()
            json_object = dict()
            json_object["m"] = dict()
            datas = json_object["m"]
            datas["v"] = list()
            datas["sflag"] = 1 #開始フラグ
            datas["vNum"] = len(self.dumpJson["m"]["v"])#頂点数
            index = 0
            for object in self.dumpJson["m"]["v"]:
                datas["v"].append(object)
                if(index >= 256):
                    index=0
                    sendDatas.append(copy.deepcopy(json_object))
                    json_object["m"].clear()
                    datas = json_object["m"]
                    datas["v"] = list()
                else : 
                    index += 1
            if(len(json_object["m"]["v"]) != 0):
                sendDatas.append(json_object.copy())
            for i in range(0,len(sendDatas)):
                if(i == len(sendDatas)-1):
                    sendDatas[i]["m"]["sflag"] = -1
                
                json_text = json.dumps(sendDatas[i],ensure_ascii=False,cls=json.JSONEncoder)
                message = json_text + "\0"
                sock.sendto(message.encode('utf-8'), serv_address)
                time.sleep(0.001)
            self.dumpJson.clear()

    #id : ノードの各ステージないでの通し番号
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
            #三角面の時
            if( len(face.loops) == 3):
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
            #一旦四角面で対応できるか
            else :
                for i in range(0,len(face.loops)-2):
                    for j in range(0,3) :
                        loopIndex = i
                        if(j==0):
                            loopIndex = 0
                        vertex = face.loops[j+loopIndex].vert
                        uv = face.loops[j+loopIndex][uv_layer].uv

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

    def SendChangingData(self,old):
        index = 0
        json_object = dict()
        json_object["m"] = dict()
        datas = json_object["m"]
        datas["v"] = list()
        
        #for object in self.json_object_root["m"]["v"]:
        #    if object != old["m"]["v"][index]:
        #        datas["v"].append(object)
        #    index = index + 1

        #self.dumpJson = json_object
        self.dumpJson = self.json_object_root
        #json_text = json.dumps(self.json_object_root,ensure_ascii=False,cls=json.JSONEncoder)
        #json_text = json.dumps(json_object,ensure_ascii=False,cls=json.JSONEncoder)

        #classのデータにコピー
        #self.jData = json_text

isSend = 0

class MYADDON_OT_export_scene(bpy.types.Operator,bpy_extras.io_utils.ExportHelper):
    bl_idname = "myadddon.myaddon_ot_export_scene"
    bl_label = "シーン出力"
    bl_description = "シーン情報をExportします"
    #出力するファイルの拡張子
    filename_ext = ".json"

    def execute(self,context):
        print("シーン情報をExportします")
        #print(bpy.context.scene.objects)

        #self.export()
        self.export_json()
        #sjson = SendJson()
        #sjson.export_json()
        #sjson.Send()
        print("シーン情報をExportしました")

        self.report({'INFO'},"シーン情報をExportしました")

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
    
    def export(self):
        """ファイルに出力"""

        print("シーン出力開始... %r" % self.filepath)

        with open(self.filepath,"wt") as file:
            self.write_and_print(file,"SCENE")

            for object in bpy.context.scene.objects:
                if object.parent:
                    continue
                self.prase_scene_recursive(file,object,0)

    def export_json(self):
        """JSON形式でファイル出力"""
        #保存する情報をまとめる
        json_object_root = dict()

        #ノード名
        json_object_root["name"] = "scene"
        #オブジェクトリストを作成
        json_object_root["objects"] = list()

        #シーン内の全オブジェクト走査してパック
        #シーン内の全オブジェクトについて
        id = 0
        for object in bpy.context.scene.objects:
            #親オブジェクトがあるものはスキップ 
            if object.parent:
                continue
            #シーン直下のオブジェクトをルートノードとし再帰関数で走査
            self.parse_scene_recursive_json(json_object_root["objects"],object,0,id)

            id+=1

        #オブジェクトをJSON文字列にエンコード
        json_text = json.dumps(json_object_root,ensure_ascii=False,cls=json.JSONEncoder,indent=4)
        #コンソールに表示する
        print(json_text)

        #ファイルオープン
        with open(self.filepath,"wt",encoding="utf-8") as file:
            file.write(json_text)
    #id : ノードの各ステージないでの通し番号
    def parse_scene_recursive_json(self,data_parent,object,level,id):

        #シーンのオブジェクト一個分のjsonオブジェクト作成
        json_object = dict()
        json_object["id"] = id
        json_object["type"] = object.type
        json_object["name"] = object.name

        #その他情報パック
        trans, rot, scale = object.matrix_local.decompose()
        #rot
        rot = rot.to_euler()
        #rot.x = math.degrees(rot.x)
        #rot.y = math.degrees(rot.y)
        #rot.z = math.degrees(rot.z)
        #トランスフォーム情報をディクショナリに登録
        transform = dict()
        transform["translation"] = (trans.x,trans.y,trans.z)
        transform["rotation"] = (rot.x,rot.y,rot.z)
        transform["scaling"] = (scale.x,scale.y,scale.z)
        #まとめて一個分のjsonオブジェクトに登録
        json_object["transform"] = transform


        if json_object["name"] == "field":
            self.export_field_vertex(object,json_object)

        #カスタムプロパティ 'file_name'
        if "file_name" in object:
            json_object["file_name"] = object["file_name"]

        #カスタムプロパティ 'collider'
        if  "collider" in object:
            collider = dict()
            collider["type"] = object["collider"]
            collider["center"] = object["collider_center"].to_list()
            collider["size"] = object["collider_size"].to_list()
            json_object["collider"] = collider

        

        #一個分のjsonオブジェクトを親オブジェクトに登録
        data_parent.append(json_object)

        #直接の子供リストを走査
        if len(object.children) > 0:
            #子ノードリストを作成
            json_object["children"] = list()
            cid=0
            #子ノードへ進むs
            for child in object.children:
                self.parse_scene_recursive_json(json_object["children"],child,level + 1,cid)
                cid+=1

    def export_field_vertex(self,object,json):
        json_object_f = dict()
        #ノード名
        #json_object_f["name"] = "field"
        #オブジェクトリストを作成
        json_object_f["vertex"] = list()
        #length = len(object.data.vertices["pos"])
        for vertex in object.data.vertices:   
            json_object = dict() 
            json_object["pos"] = (vertex.co.x,vertex.co.y,vertex.co.z)
            json_object["normal"] = (vertex.normal.x,vertex.normal.y,vertex.normal.z)
            
            #まとめて一個分のjsonオブジェクトに登録
            json_object_f["vertex"].append(json_object)
        
        json["vertices"] = json_object_f

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
        self.layout.operator(MYADDON_OT_stretch_vertex.bl_idname, text=MYADDON_OT_stretch_vertex.bl_label)
        self.layout.operator(MYADDON_OT_create_ico_sphere.bl_idname, text=MYADDON_OT_create_ico_sphere.bl_label)
        self.layout.operator(MYADDON_OT_export_scene.bl_idname, text=MYADDON_OT_export_scene.bl_label)
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

class MYADDON_OT_add_collider(bpy.types.Operator):
    bl_idname = "myaddon.myaddon_ot_add_collider"
    bl_label = "コライダー 追加"
    bl_description = "['collider']カスタムプロパティを追加します"
    bl_options = {"REGISTER","UNDO"}

    def execute(self,context):

        context.object["collider"] = "BOX"
        context.object["collider_center"] = mathutils.Vector((0,0,0))
        context.object["collider_size"] = mathutils.Vector((2,2,2))

        return {"FINISHED"}

class OBJECT_PT_collider(bpy.types.Panel):
    """オブジェクトのコライダーパネル"""
    bl_idname = "OBJECT_PT_collider"
    bl_label = "collider"
    bl_space_type = "PROPERTIES"
    bl_region_type = "WINDOW"
    bl_context = "object"
    
    def draw(self,context):
        if "collider" in context.object:
            self.layout.prop(context.object,'["collider"]',text="Type")
            self.layout.prop(context.object,'["collider_center"]',text="Center")
            self.layout.prop(context.object,'["collider_size"]',text="Size")

        else:
            self.layout.operator(MYADDON_OT_add_collider.bl_idname)

#Blenderに登録するクラスリスト
classes = (
    MYADDON_OT_stretch_vertex,
    MYADDON_OT_create_ico_sphere,
    MYADDON_OT_export_scene,
    MYADDON_OT_export_meshsyncobject,
    TOPBAR_MT_my_menu,
    MYADDON_OT_add_filename,
    OBJECT_PT_file_name,
    MYADDON_OT_add_collider,
    OBJECT_PT_collider,
)

class DrawCollider:
    handle = None
    
    def draw_Box(object):
        vertices = {"pos":[]}
        indices = []

        offsets = [
            [-0.5,-0.5,-0.5],
            [+0.5,-0.5,-0.5],
            [-0.5,+0.5,-0.5],
            [+0.5,+0.5,-0.5],
            [-0.5,-0.5,+0.5],
            [+0.5,-0.5,+0.5],
            [-0.5,+0.5,+0.5],
            [+0.5,+0.5,+0.5]
        ]

        #size = [2,2,2]

    

        center = mathutils.Vector((0,0,0))
        size = mathutils.Vector((2,2,2))

        center[0] = object["collider_center"][0]
        center[1] = object["collider_center"][1]
        center[2] = object["collider_center"][2]
        size[0] = object["collider_size"][0]
        size[1] = object["collider_size"][1]
        size[2] = object["collider_size"][2]

        start = len(vertices["pos"])

        for offset in offsets:
            
            pos = copy.copy(center)

            pos[0] += offset[0]*size[0]
            pos[1] += offset[1]*size[1]
            pos[2] += offset[2]*size[2] 
            pos = object.matrix_world @ pos

            vertices['pos'].append(pos)
            #前面
            indices.append([start+0,start+1])
            indices.append([start+2,start+3])
            indices.append([start+0,start+2])
            indices.append([start+1,start+3])
            #奥面
            indices.append([start+4,start+5])
            indices.append([start+6,start+7])
            indices.append([start+4,start+6])
            indices.append([start+5,start+7])
            #前と頂点をつなぐ辺
            indices.append([start+0,start+4])
            indices.append([start+1,start+5])
            indices.append([start+2,start+6])
            indices.append([start+3,start+7])

        #ビルトインのシェーダ取得
        shader = gpu.shader.from_builtin("3D_UNIFORM_COLOR")

        batch = gpu_extras.batch.batch_for_shader(shader,"LINES",vertices,indices = indices)

        color = [0.5,1.0,1.0,1.0]
        shader.bind()
        shader.uniform_float("color",color)

        #描画
        batch.draw(shader)
    
    def draw_Sphere(object):
        vertices = {"pos":[]}
        indices = []

        #size = [2,2,2]

    

        center = mathutils.Vector((0,0,0))
        size = mathutils.Vector((2,2,2))

        center[0] = object["collider_center"][0]
        center[1] = object["collider_center"][1]
        center[2] = object["collider_center"][2]
        size[0] = object["collider_size"][0]
        size[1] = object["collider_size"][1]
        size[2] = object["collider_size"][2]

        start = len(vertices["pos"])

        kSubdivision = 16
        kLonEvery = float(3.141592) / float(kSubdivision) * 2.0
        kLatEvery = float(3.141592) / float(kSubdivision)

        radius = size[0] * 0.5

        for latIndex in range(0,kSubdivision,1):
            lat = -float(3.141592) / 2.0 + kLatEvery * latIndex
            for lonIndex in  range(0,kSubdivision,1):
                lon = lonIndex * kLonEvery
                a = mathutils.Vector((0,0,0))
                b = mathutils.Vector((0,0,0))
                c = mathutils.Vector((0,0,0))
                a[0] = radius * math.cos(lat) * math.cos(lon)
                a[1] = radius * math.sin(lat)
                a[2] = radius * math.cos(lat) * math.sin(lon)
               
                b[0] = radius * math.cos(lat + kLatEvery) * math.cos(lon)
                b[1] = radius * math.sin(lat + kLatEvery)
                b[2] = radius * math.cos(lat + kLatEvery) * math.sin(lon)  
               
                c[0] = radius * math.cos(lat) * math.cos(lon + kLonEvery)
                c[1] = radius * math.sin(lat)
                c[2] = radius * math.cos(lat) * math.sin(lon + kLonEvery)

                a = object.matrix_world @ a
                b = object.matrix_world @ b
                c = object.matrix_world @ c

                vertices["pos"].append(a)
                vertices["pos"].append(b)
                vertices["pos"].append(c)

                first = (latIndex * kSubdivision + (lonIndex )) * 3

                indices.append([first,first+1])
                indices.append([first,first+2])

        #ビルトインのシェーダ取得
        shader = gpu.shader.from_builtin("3D_UNIFORM_COLOR")

        batch = gpu_extras.batch.batch_for_shader(shader,"LINES",vertices,indices = indices)

        color = [0.5,1.0,1.0,1.0]
        shader.bind()
        shader.uniform_float("color",color)

        #描画
        batch.draw(shader)

    def draw_collider():
        for object in bpy.context.scene.objects:

            if not "collider" in object:
                continue
            # collidertypeがboxだったら
            if object["collider"] == "BOX":
                DrawCollider.draw_Box(object)

            if object["collider"] == "SPHERE":
                DrawCollider.draw_Sphere(object)

isSend = 1
isSendFileld = 1

def send():
    sjson = SendJson()
    #jdata = sjson.jData.copy()
    jdata = copy.deepcopy(sjson.jData)
    while isSend==1:
        time.sleep(0.1)
        sjson = SendJson()
        sjson.export_json()
        if jdata != sjson.jData:
            sjson.Send()
            jdata = copy.deepcopy(sjson.jData)

def sendF():
    sjson = SendField()
    sjson.export_json()
    #jdata = sjson.jData.copy()
    #起動時の状態をコピーする
    jdata = copy.deepcopy(sjson.json_object_root)
    while isSend==1:
        time.sleep(0.1)
        sjson = SendField()
        sjson.export_json()
        if jdata != sjson.json_object_root:
            sjson.SendChangingData(jdata)
            jdata = copy.deepcopy(sjson.json_object_root)
            sjson.Send()

#thread1 = threading.Thread()
threads = []

#アドオン有効化時コールバックS
def register():
    #Blenderにクラスを登録
    for cls in classes:
        bpy.utils.register_class(cls)

    #メニューに項目追加
    bpy.types.TOPBAR_MT_editor_menus.append(TOPBAR_MT_my_menu.submenu)
    #3Dビューに描画関数追加
    DrawCollider.handle = bpy.types.SpaceView3D.draw_handler_add(DrawCollider.draw_collider,(),"WINDOW","POST_VIEW")
    print("レベルエディタが有効化されました。")
    

    global isSend
    isSend = 1
    global isSendField
    isSendField = 1

    thread1 = threading.Thread(target=send)
    thread2 = threading.Thread(target=sendF)
    threads.append(thread1)
    threads.append(thread2)
    thread1.start()
    thread2.start()
    #asyncio.run(send(jdata,sjson))

#アドオン無効化時コールバック
def unregister():
    #メニューから項目削除
    bpy.types.TOPBAR_MT_editor_menus.remove(TOPBAR_MT_my_menu.submenu)
    #3Dビューから描画関数を削除
    bpy.types.SpaceView3D.draw_handler_remove(DrawCollider.handle,"WINDOW")
    
    # Blenderからクラスを削除
    for cls in classes:
        bpy.utils.unregister_class(cls)
    
    print("レベルエディタが無効化されました。")
    global isSend
    isSend = 0
    global isSendField
    isSendField = 0
    for t in threads:
        t.join()

if __name__ == "__main__":
    register()