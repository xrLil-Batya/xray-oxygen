--// FXAA -------------------------------------------------------------------------------------------
function element_0(shader, t_base, t_second, t_detail)		-- [0] FXAA: pass 0 - luminance calculation
	shader:begin("stub_screen_space", "fxaa_luma")
		:fog			(false)
		:zb				(false, false)
	shader:dx10texture	("s_image",		"$user$albedo")
	shader:dx10sampler	("smp_rtlinear")
end

function element_1(shader, t_base, t_second, t_detail)		-- [1] FXAA: pass 1 - actually antialiasing
	shader:begin("stub_screen_space", "fxaa_main")
		:fog			(false)
		:zb				(false, false)
	shader:dx10texture	("s_image",		"$user$generic0")

	shader:dx10sampler	("smp_rtlinear")
end

--// SMAA -------------------------------------------------------------------------------------------
function element_2(shader, t_base, t_second, t_detail)		-- [2] SMAA: pass 0 - edge detection
	shader:begin("stub_screen_space", "smaa_edge_detect")
		:fog			(false)
		:zb				(false, false)
	shader:dx10texture	("s_image", 	"$user$albedo")
	shader:dx10texture	("s_position", 	"$user$position")
end

function element_3(shader, t_base, t_second, t_detail)		-- [3] SMAA: pass 1 - blending weight calculation
	shader:begin("stub_screen_space", "smaa_bweight_calc")
		:fog			(false)
		:zb				(false, false)
	shader:dx10texture	("s_edgetex", 	"$user$smaa_edgetex")
	shader:dx10texture	("s_areatex", 	"shaders\\smaa_area_tex_dx10")
	shader:dx10texture	("s_searchtex", "shaders\\smaa_search_tex")
end

function element_4(shader, t_base, t_second, t_detail)		-- [4] SMAA: pass 2 - neighbourhood blending
	shader:begin("stub_screen_space", "smaa_neighbour_blend")
		:fog			(false)
		:zb				(false, false)
	shader:dx10texture	("s_image", 	"$user$albedo")
	shader:dx10texture	("s_blendtex", 	"$user$smaa_blendtex")
end

--// DLAA -------------------------------------------------------------------------------------------
function element_5(shader, t_base, t_second, t_detail)		-- [5] DLAA: single pass
    shader:begin("stub_screen_space", "dlaa_main")
		:fog			(false)
		:zb				(false, false)
	shader:dx10texture	("s_image",		"$user$albedo")
	
	shader:dx10sampler	("smp_rtlinear")
end