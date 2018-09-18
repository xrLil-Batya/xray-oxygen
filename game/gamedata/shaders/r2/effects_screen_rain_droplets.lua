local tex_image		= "$user$albedo"
local tex_droplets	= "fx\\fx_hud_droplets"

function element_0(shader, t_base, t_second, t_detail)
	shader:begin("null", "rain_drops")
		:fog		(false)
		:zb			(false, false)
	shader:sampler	("s_image")		:texture(tex_image)		:clamp() :f_none()
	shader:sampler	("s_droplets")	:texture(tex_droplets)	:clamp() :f_linear()
end