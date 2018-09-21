function element_0(shader, t_base, t_second, t_detail)		-- [0] FXAA: pass 0 - luminance calculation
	shader:begin("null", "fxaa_luma")
		:fog		(false)
		:zb			(false, false)
	shader:sampler	("s_image") :texture("$user$albedo") :clamp() :f_none()
end

function element_1(shader, t_base, t_second, t_detail)		-- [1] FXAA: pass 1 - actually antialiasing
	shader:begin("null", "fxaa_main")
		:fog		(false)
		:zb			(false, false)
	shader:sampler	("s_image")	:texture("$user$generic0") :clamp() :f_linear()
end