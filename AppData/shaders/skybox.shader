{
	"vert" : "skybox.vert",
	"frag" : "skybox.frag",
	"uniforms_blocks" : [
		{
			"name" : "UboObject",
			"binding" : 1,
			"size" : 108,
			"stageFlags" : 17,
			"type" : 0,
			"uniforms" : [
				{
					"name" : "baseColor",
					"binding" : -1,
					"offset" : 64,
					"size" : 16,
					"glType" : 35666,
					"readOnly" : false,
					"writeOnly" : false,
					"stageFlags" : 16
				},
				{
					"name" : "blendFactor",
					"binding" : -1,
					"offset" : 104,
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
			"name" : "samplerColor",
			"binding" : 2,
			"offset" : -1,
			"size" : -1,
			"glType" : 35680,
			"readOnly" : false,
			"writeOnly" : false,
			"stageFlags" : 16
		}
	],
	"attributes" : [
		{
			"name" : "inPosition",
			"set": 63,
			"location" : 0,
			"size" : 2,
			"glType" : 35665
		}
	]
}