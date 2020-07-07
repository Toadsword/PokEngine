{
	"comp" : "prefiltered.comp",
	"local_size_x" : 16,
	"local_size_y" : 16,
	"local_size_z" : 0,
	"uniforms_blocks" : [
		{
			"name" : "PushObject",
			"binding" : -1,
			"size" : 4,
			"stageFlags" : 32,
			"type" : 2,
			"uniforms" : [
				{
					"name" : "roughness",
					"binding" : -1,
					"offset" : 0,
					"size" : 4,
					"glType" : 5126,
					"readOnly" : false,
					"writeOnly" : false,
					"stageFlags" : 32
				}
			]
		}
	],
	"uniforms" : [
		{
			"name" : "outColor",
			"binding" : 0,
			"offset" : -1,
			"size" : -1,
			"glType" : 36944,
			"readOnly" : false,
			"writeOnly" : true,
			"stageFlags" : 32
		},
		{
			"name" : "samplerColor",
			"binding" : 1,
			"offset" : -1,
			"size" : -1,
			"glType" : 35680,
			"readOnly" : false,
			"writeOnly" : false,
			"stageFlags" : 32
		}
	]
}