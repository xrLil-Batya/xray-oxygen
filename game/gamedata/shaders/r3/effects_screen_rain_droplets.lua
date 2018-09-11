local tex_image		= "$user$albedo"
local tex_droplets	= "fx\\fx_hud_droplets"

function element_0(shader, t_base, t_second, t_detail)
	shader:begin("stub_screen_space", "rain_drops")
		:fog			(false)
		:zb				(false, false)
	shader:dx10texture	("s_image",		tex_image)
	shader:dx10texture	("s_droplets",	tex_droplets)

	shader:dx10sampler	("smp_rtlinear")
end