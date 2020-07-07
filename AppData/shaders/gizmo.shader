{
	"vert" : "gizmo.vert",
	"frag" : "gizmo.frag",
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
		}
	],
	"attributes" : [
		{
			"name" : "inColor",
			"set": 63,
			"location" : 7,
			"size" : 16,
			"glType" : 35666
		},
		{
			"name" : "inModelMatrix",
			"set": 63,
			"location" : 3,
			"size" : 64,
			"glType" : 35676
		},
		{
			"name" : "inPosition",
			"set": 63,
			"location" : 0,
			"size" : 12,
			"glType" : 35666
		}
	]
}