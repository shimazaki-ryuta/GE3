
float32_t3 RGBToHSV(float32_t3 rgb){
	float max = rgb.r > rgb.g ? rgb.r : rgb.g;
	max = max > rgb.b ? max : rgb.b;
	float min = rgb.r < rgb.g ? rgb.r : rgb.g;
	min = min < rgb.b ? min : rgb.b;

	float h = max - min;
	if(h > 0.0f){
		if(max == rgb.r){
			h = (rgb.g-rgb.b) / h;
			if(h < 0.0f){
				h += 6.0f;
			}
		}
		else if(max == rgb.g){
			h = 2.0f + (rgb.b-rgb.r) / h;
		}
		else{
			h = 4.0f + (rgb.r-rgb.g) / h;
		}
	}
	h /= 6.0f;
	float s = (max - min);
	if(max != 0.0f){
		s /= max;
	}
	float32_t3 result;
	result.x = h;
	result.y = s;
	result.z = max;
	return result;
}

float32_t3 HSVToRGB(float32_t3 hsv){
	float h = hsv.r;
	float s = hsv.g;
	float v = hsv.b;
	float32_t3 result;
	result.r = v;
	result.g = v;
	result.b = v;

	if(s > 0.0f){
		h *= 6.0f;
		int32_t i = (int)h;
		float f = h - (float)i;
		switch (i){
			default:
			case 0:
				result.g *= 1.0f - s*(1.0f - f);
				result.b *= 1.0f - s;
				break;
			case 1:
				result.r *= 1.0f - s*f;
				result.b *= 1.0f - s;
				break;
			case 2:
				result.r *= 1.0f - s;
				result.b *= 1.0f - s * (1.0f - f);
				break;
			case 3:
				result.r *= 1.0f - s;
				result.g *= 1.0f - s * f;
				break;
			case 4:
				result.r *= 1.0f - s * (1.0f - f);
				result.g *= 1.0f - s;
				break;
			case 5:
				result.g *= 1.0f - s;
				result.b *= 1.0f - s * f;
				break;
		}
	}
	return result;
}