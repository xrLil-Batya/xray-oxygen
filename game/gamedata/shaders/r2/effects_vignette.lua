function element_0(shader, t_base, t_second, t_detail)
	shader:begin("null", "vignette")
		:fog		(false)
		:zb			(false, false)
	shader:sampler	("s_image") 	:texture("$user$albedo")	:clamp() :f_none()
end
