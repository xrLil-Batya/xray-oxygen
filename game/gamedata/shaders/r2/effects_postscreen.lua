function element_2(shader, t_base, t_second, t_detail)
	shader:begin("null", "vignette")
		:fog		(false)
		:zb			(false, false)
	shader:sampler	("s_image") 	:texture("$user$albedo")	:clamp() :f_none()
end

--// Chromatic Aberration
function element_0(shader, t_base, t_second, t_detail)
	shader:begin("null", "chromatic_aberration")
		:fog			(false)
		:zb				(false, false)
	shader:sampler	("s_image") 	:texture("$user$albedo")	:clamp() :f_none()
end

--// Color grading
local tex_color	= "shaders\\color_chart"
function element_1(shader, t_base, t_second, t_detail)
	shader:begin("null", "color_grading")
		:fog			(false)
		:zb				(false, false)
	shader:sampler	("s_image") 	        :texture("$user$albedo")	:clamp() :f_none()
	shader:sampler	("color_chart_sampler")	:texture(tex_color)	 	    :clamp() :f_linear()
end