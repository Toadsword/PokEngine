{
	"vert" : "diffuse.vert",
	"frag" : "diffuse_bc.frag",
	"uniforms_blocks" : [
		{
			"name" : "UniformScene",
			"binding" : 0,
			"size" : 128,
			"stageFlags" : 1,
			"type" : 0,
			"uniforms" : [
				{
					"name" : "projection",
					"binding" : -1,
					"offset" : 0,
					"size" : 64,
					"glType" : 35676,
					"readOnly" : false,
					"writeOnly" : false,
					"stageFlags" : 1
				},
				{
					"name" : "view",
					"binding" : -1,
					"offset" : 64,
					"size" : 64,
					"glType" : 35676,
					"readOnly" : false,
					"writeOnly" : false,
					"stageFlags" : 1
				}
			]
		},
		{
			"name" : "UniformObject",
			"binding" : 1,
			"size" : 24,
			"stageFlags" : 16,
			"type" : 0,
			"uniforms" : [
				{
					"name" : "color",
					"binding" : -1,
					"offset" : 0,
					"size" : 16,
					"glType" : 35666,
					"readOnly" : false,
					"writeOnly" : false,
					"stageFlags" : 16
				},
				{
					"name" : "roughness",
					"binding" : -1,
					"offset" : 16,
					"size" : 4,
					"glType" : 5126,
					"readOnly" : false,
					"writeOnly" : false,
					"stageFlags" : 16
				},
				{
					"name" : "metallic",
					"binding" : -1,
					"offset" : 20,
					"size" : 4,
					"glType" : 5126,
					"readOnly" : false,
					"writeOnly" : false,
					"stageFlags" : 16
				}
			]
		}
	],
	"uniforms" : [
		{
			"name" : "samplerBaseColor",
			"binding" : 2,
			"offset" : -1,
			"size" : -1,
			"glType" : 35678,
			"readOnly" : false,
			"writeOnly" : false,
			"stageFlags" : 16
		}
	],
	"attributes" : [
		{
			"name" : "inNormal",
			"set": 63,
			"location" : 2,
			"size" : 12,
			"glType" : 35666
		},
		{
			"name" : "inPosition",
			"set": 63,
			"location" : 0,
			"size" : 12,
			"glType" : 35666
		},
		{
			"name" : "inUV",
			"set": 63,
			"location" : 1,
			"size" : 8,
			"glType" : 35664
		},
		{
			"name" : "inModelMatrix",
			"set": 63,
			"location" : 3,
			"size" : 64,
			"glType" : 35676
		}
	]
}