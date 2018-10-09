function element_0(shader, t_base, t_second, t_detail)		-- [0] transfer into bloom-target
	shader:begin("stub_notransform_build", "bloom_build")
		:blend			(false, blend.srcalpha, blend.invsrcalpha)
		:fog			(false)
		:zb				(false, false)
	shader:dx10texture	("s_image", "$user$generic1")
	
	shader:dx10sampler	("smp_rtlinear")
end

function element_1(shader, t_base, t_second, t_detail)		-- [1] X-filter
	shader:begin("stub_notransform_filter", "bloom_filter")
		:fog			(false)
		:zb				(false, false)
	shader:dx10texture	("s_bloom", "$user$bloom1")
	
	shader:dx10sampler	("smp_rtlinear")
end

function element_2(shader, t_base, t_second, t_detail)		-- [2] Y-filter
	shader:begin("stub_notransform_filter", "bloom_filter")
		:fog			(false)
		:zb				(false, false)
	shader:dx10texture	("s_bloom", "$user$bloom2")
	
	shader:dx10sampler	("smp_rtlinear")
end

function element_3(shader, t_base, t_second, t_detail)		-- [3] FF-filter_P0
	shader:begin("stub_notransform_build", "bloom_filter_f")
		:fog			(false)
		:zb				(false, false)
	shader:dx10texture	("s_bloom", "$user$bloom1")
	
	shader:dx10sampler	("smp_rtlinear")
end

function element_4(shader, t_base, t_second, t_detail)		-- [4] FF-filter_P1
	shader:begin("stub_notransform_build", "bloom_filter_f")
		:fog			(false)
		:zb				(false, false)
	shader:dx10texture	("s_bloom", "$user$bloom2")
	
	shader:dx10sampler	("smp_rtlinear")
end