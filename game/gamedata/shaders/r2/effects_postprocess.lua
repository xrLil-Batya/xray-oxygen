function element_0(shader, t_base, t_second, t_detail)
	shader:begin("null", "postprocess")
			: fog	(false)
			: zb 	(false,false)
	shader:sampler	("s_base0")		:texture("$user$albedo")		:clamp() 	:f_linear()
	shader:sampler	("s_base1")    	:texture("$user$albedo")		:clamp() 	:f_linear()
	shader:sampler	("s_noise")    	:texture("fx\\fx_noise2")					:f_linear()
end

function element_1(shader, t_base, t_second, t_detail)
	shader:begin("null", "postprocess_CM")
			: fog	(false)
			: zb 	(false,false)
	shader:sampler	("s_base0")		:texture("$user$albedo")		:clamp() 	:f_linear()
	shader:sampler	("s_base1")    	:texture("$user$albedo")		:clamp() 	:f_linear()
	shader:sampler	("s_noise")    	:texture("fx\\fx_noise2")					:f_linear()
	shader:sampler	("s_grad0")    	:texture("$user$cmap0")			:clamp() 	:f_linear()
	shader:sampler	("s_grad1")    	:texture("$user$cmap1")			:clamp() 	:f_linear()
end
