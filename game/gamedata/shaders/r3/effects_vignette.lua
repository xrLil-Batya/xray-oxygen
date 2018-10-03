function element_0(shader, t_base, t_second, t_detail)
	shader:begin("stub_screen_space", "vignette")
		:fog			(false)
		:zb				(false, false)
	shader:dx10texture	("s_image", "$user$albedo")
end
