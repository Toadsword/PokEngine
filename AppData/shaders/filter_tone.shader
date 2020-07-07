	{
	"vert" : "filter_default.vert",
	"frag" : "filter_tone.frag",
	"uniforms" : [
		{
			"name" : "samplerColor",
			"binding" : 1,
			"offset" : -1,
			"size" : -1,
			"glType" : 35678,
			"readOnly" : false,
			"writeOnly" : false,
			"stageFlags" : 16
		},
		{
			"name" : "writeColor",
			"binding" : 0,
			"offset" : -1,
			"size" : -1,
			"glType" : 36941,
			"readOnly" : false,
			"writeOnly" : true,
			"stageFlags" : 16
		}
	],
	"attributes" : [
		{
			"name" : "gl_VertexIndex",
			"set": 63,
			"location" : 4095,
			"size" : 4,
			"glType" : 5124
		}
	]
}