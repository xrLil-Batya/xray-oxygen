function element_0(shader, t_base, t_second, t_detail)		--[0]
	shader:begin("stub_notransform_postpr","postprocess")
		:blend			(false, blend.srcalpha, blend.invsrcalpha)
		:fog			(false)
		:zb 			(false, false)
	shader:dx10texture	("s_base0", "$user$generic")
	shader:dx10texture	("s_base1", "$user$generic")
	shader:dx10texture	("s_noise", "fx\\fx_noise2")

	shader:dx10sampler	("smp_rtlinear")
	shader:dx10sampler	("smp_linear")
end

function element_1(shader, t_base, t_second, t_detail)		--[1] use color map
	shader:begin("stub_notransform_postpr","postprocess_CM")
		:blend			(false, blend.srcalpha, blend.invsrcalpha)
		:fog			(false)
		:zb 			(false, false)
	shader:dx10texture	("s_base0", "$user$generic")
	shader:dx10texture	("s_base1", "$user$generic")
	shader:dx10texture	("s_noise", "fx\\fx_noise2")
	shader:dx10texture	("s_grad0", "$user$cmap0")
	shader:dx10texture	("s_grad1", "$user$cmap1")

	shader:dx10sampler	("smp_rtlinear")
	shader:dx10sampler	("smp_linear")
end