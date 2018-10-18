function element_0(shader, t_base, t_second, t_detail)		-- [0] transfer into bloom-target
	shader:begin("null", "bloom_build")
		:blend		(false, blend.srcalpha, blend.invsrcalpha)
		:fog		(false)
		:zb			(false, false)
	shader:sampler	("s_image") 	:texture("$user$generic1")	:clamp() :f_linear()
end

function element_1(shader, t_base, t_second, t_detail)		-- [1] X-filter
	shader:begin("null", "bloom_filter")
		:fog		(false)
		:zb			(false, false)
	shader:sampler	("s_bloom")		:texture("$user$bloom1")	:clamp() :f_linear()
end

function element_2(shader, t_base, t_second, t_detail)		-- [2] Y-filter
	shader:begin("null", "bloom_filter")
		:fog		(false)
		:zb			(false, false)
	shader:sampler	("s_bloom")		:texture("$user$bloom2")	:clamp() :f_linear()
end

function element_3(shader, t_base, t_second, t_detail)		-- [3] FF-filter_P0
	shader:begin("null", "bloom_filter_f")
		:fog		(false)
		:zb			(false, false)
	shader:sampler	("s_bloom")		:texture("$user$bloom1")	:clamp() :f_linear()
end

function element_4(shader, t_base, t_second, t_detail)		-- [4] FF-filter_P1
	shader:begin("null", "bloom_filter_f")
		:fog		(false)
		:zb			(false, false)
	shader:sampler	("s_bloom")		:texture("$user$bloom2")	:clamp() :f_linear()
end