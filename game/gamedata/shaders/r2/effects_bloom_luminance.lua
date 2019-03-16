function element_0(shader, t_base, t_second, t_detail)		-- [0] 256x256	=> 64x64
	shader:begin("null", "bloom_luminance_1")
		:fog		(false)
		:zb			(false, false)
	shader:sampler	("s_image") 	:texture("$user$bloom1")		:clamp() :f_linear()
end

function element_1(shader, t_base, t_second, t_detail)		-- [1] 64x64	=> 8x8
	shader:begin("null", "bloom_luminance_2")
		:fog		(false)
		:zb			(false, false)
	shader:sampler	("s_image")		:texture("$user$lum_t64")		:clamp() :f_linear()
end

function element_2(shader, t_base, t_second, t_detail)		-- [2] 8x8		=> 1x1, blending with old result
	shader:begin("null", "bloom_luminance_3")
		:fog		(false)
		:zb			(false, false)
	shader:sampler	("s_image")		:texture("$user$lum_t8")		:clamp() :f_linear()
	shader:sampler	("s_tonemap")	:texture("$user$tonemap_src")	:clamp() :f_linear()
end
