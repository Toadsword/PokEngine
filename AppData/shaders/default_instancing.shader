{
	"vert" : "default_instancing.vert",
	"frag" : "default_instancing.frag",
	"uniforms_blocks" : [
		{
			"name" : "UboScene",
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
		}
	],
	"uniforms" : [
		{
			"name" : "samplerDiffuse",
			"binding" : 1,
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