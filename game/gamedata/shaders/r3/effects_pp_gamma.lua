function element_0(shader, t_base, t_second, t_detail)		-- [0] pass 0 - LUT Generation
	shader:begin("stub_screen_space", "gamma_gen_lut")
		:fog			(false)
		:zb				(false, false)
end

function element_1(shader, t_base, t_second, t_detail)		-- [1] pass 1 - Apply LUT
	shader:begin("stub_screen_space", "gamma_apply")
		:fog			(false)
		:zb				(false, false)
	shader:dx10texture	("s_image",		"$user$generic0")
	shader:dx10texture	("s_gamma_lut",	"$user$gamma_lut")

	shader:dx10sampler	("smp_nofilter")
	shader:dx10sampler	("smp_rtlinear")
end