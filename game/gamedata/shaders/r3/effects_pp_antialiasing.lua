function element_0(shader, t_base, t_second, t_detail)		-- [0] FXAA: pass 0 - luminance calculation
	shader:begin("stub_screen_space", "fxaa_luma")
		:fog			(false)
		:zb				(false, false)
	shader:dx10texture	("s_image",		"$user$albedo")
end

function element_1(shader, t_base, t_second, t_detail)		-- [1] FXAA: pass 1 - actually antialiasing
	shader:begin("stub_screen_space", "fxaa_main")
		:fog			(false)
		:zb				(false, false)
	shader:dx10texture	("s_image",		"$user$generic0")

	shader:dx10sampler	("smp_rtlinear")
end