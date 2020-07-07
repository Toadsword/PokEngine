{
	"vert" : "default.vert",
	"frag" : "default.frag",
	"uniforms_blocks" : [
		{
			"name" : "UboObject",
			"binding" : 1,
			"size" : 96,
			"stageFlags" : 17,
			"type" : 0,
			"uniforms" : [
				{
					"name" : "baseDiffuse",
					"binding" : -1,
					"offset" : 64,
					"size" : 16,
					"glType" : 35666,
					"readOnly" : false,
					"writeOnly" : false,
					"stageFlags" : 16
				},
				{
					"name" : "ignoreFog",
					"binding" : -1,
					"offset" : 88,
					"size" : 4,
					"glType" : 5126,
					"readOnly" : false,
					"writeOnly" : false,
					"stageFlags" : 16
				},
				{
					"name" : "ignoreLighting",
					"binding" : -1,
					"offset" : 92,
					"size" : 4,
					"glType" : 5126,
					"readOnly" : false,
					"writeOnly" : false,
					"stageFlags" : 16
				},
				{
					"name" : "metallic",
					"binding" : -1,
					"offset" : 80,
					"size" : 4,
					"glType" : 5126,
					"readOnly" : false,
					"writeOnly" : false,
					"stageFlags" : 16
				},
				{
					"name" : "roughness",
					"binding" : -1,
					"offset" : 84,
					"size" : 4,
					"glType" : 5126,
					"readOnly" : false,
					"writeOnly" : false,
					"stageFlags" : 16
				},
				{
					"name" : "transform",
					"binding" : -1,
					"offset" : 0,
					"size" : 64,
					"glType" : 35676,
					"readOnly" : false,
					"writeOnly" : false,
					"stageFlags" : 1
				}
			]
		},
		{
			"name" : "UboScene",
			"binding" : 0,
			"size" : 140,
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
				},
				{
					"name" : "cameraPos",
					"binding" : -1,
					"offset" : 128,
					"size" : 16,
					"glType" : 35666,
					"readOnly" : false,
					"writeOnly" : false,
					"stageFlags" : 1
				}
			]
		}
	],
	"uniforms" : [
		{
			"name" : "samplerDiffuse",
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
			"glType" : 35665
		},
		{
			"name" : "inPosition",
			"set": 63,
			"location" : 0,
			"size" : 12,
			"glType" : 35665
		},
		{
			"name" : "inUV",
			"set": 63,
			"location" : 1,
			"size" : 8,
			"glType" : 35664
		}
	]
}