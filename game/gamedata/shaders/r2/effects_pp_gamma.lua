function element_0(shader, t_base, t_second, t_detail)		-- [0] pass 0 - LUT Generation
	shader:begin("null", "gamma_gen_lut")
		:fog			(false)
		:zb				(false, false)
end

function element_1(shader, t_base, t_second, t_detail)		-- [1] pass 1 - Apply LUT
	shader:begin("null", "gamma_apply")
		:fog			(false)
		:zb				(false, false)
	shader:sampler		("s_image")		:texture("$user$generic0") :clamp()	:f_none()
	shader:sampler		("s_gamma_lut")	:texture("$user$gamma_lut"):clamp() :f_linear()
end