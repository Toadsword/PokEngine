{
	"vert" : "deferred.vert",
	"frag" : "deferred.frag",
	"uniforms_blocks" : [
		{
			"name" : "UniformScene",
			"binding" : 0,
			"size" : 112,
			"stageFlags" : 16,
			"type" : 0,
			"uniforms" : [
				{
					"name" : "view",
					"binding" : -1,
					"offset" : 0,
					"size" : 64,
					"glType" : 35676,
					"readOnly" : false,
					"writeOnly" : false,
					"stageFlags" : 16
				},
				{
					"name" : "cameraPosition",
					"binding" : -1,
					"offset" : 64,
					"size" : 12,
					"glType" : 35665,
					"readOnly" : false,
					"writeOnly" : false,
					"stageFlags" : 16
				},
				{
					"name" : "pointLightCount",
					"binding" : -1,
					"offset" : 76,
					"size" : 4,
					"glType" : 5124,
					"readOnly" : false,
					"writeOnly" : false,
					"stageFlags" : 16
				},
				{
					"name" : "directionalLightDirection",
					"binding" : -1,
					"offset" : 80,
					"size" : 12,
					"glType" : 35665,
					"readOnly" : false,
					"writeOnly" : false,
					"stageFlags" : 16
				},
				{
					"name" : "spotLightCount",
					"binding" : -1,
					"offset" : 92,
					"size" : 4,
					"glType" : 5124,
					"readOnly" : false,
					"writeOnly" : false,
					"stageFlags" : 16
				},
				{
					"name" : "directionalLightColor",
					"binding" : -1,
					"offset" : 96,
					"size" : 16,
					"glType" : 35666,
					"readOnly" : false,
					"writeOnly" : false,
					"stageFlags" : 16
				}
			]
		},
		{
			"name" : "BufferPointLights",
			"binding" : 1,
			"size" : 0,
			"stageFlags" : 16,
			"type" : 1,
			"uniforms" : [
				{
					"name" : "pointLights.color",
					"binding" : -1,
					"offset" : 0,
					"size" : 16,
					"glType" : 35666,
					"readOnly" : false,
					"writeOnly" : false,
					"stageFlags" : 16
				},
				{
					"name" : "pointLights.position",
					"binding" : -1,
					"offset" : 16,
					"size" : 12,
					"glType" : 35665,
					"readOnly" : false,
					"writeOnly" : false,
					"stageFlags" : 16
				},
				{
					"name" : "pointLights.radius",
					"binding" : -1,
					"offset" : 28,
					"size" : 4,
					"glType" : 5126,
					"readOnly" : false,
					"writeOnly" : false,
					"stageFlags" : 16
				}
			]
		},
		{
			"name" : "BufferSpotLights",
			"binding" : 2,
			"size" : 0,
			"stageFlags" : 16,
			"type" : 1,
			"uniforms" : [
				{
					"name" : "spotLights.angle",
					"binding" : -1,
					"offset" : 44,
					"size" : 4,
					"glType" : 5126,
					"readOnly" : false,
					"writeOnly" : false,
					"stageFlags" : 16
				},
				{
					"name" : "spotLights.color",
					"binding" : -1,
					"offset" : 0,
					"size" : 16,
					"glType" : 35666,
					"readOnly" : false,
					"writeOnly" : false,
					"stageFlags" : 16
				},
				{
					"name" : "spotLights.position",
					"binding" : -1,
					"offset" : 16,
					"size" : 12,
					"glType" : 35665,
					"readOnly" : false,
					"writeOnly" : false,
					"stageFlags" : 16
				},
				{
					"name" : "spotLights.range",
					"binding" : -1,
					"offset" : 28,
					"size" : 4,
					"glType" : 5126,
					"readOnly" : false,
					"writeOnly" : false,
					"stageFlags" : 16
				},
				{
					"name" : "spotLights.target",
					"binding" : -1,
					"offset" : 32,
					"size" : 12,
					"glType" : 35665,
					"readOnly" : false,
					"writeOnly" : false,
					"stageFlags" : 16
				}
			]
		}
	],
	"uniforms" : [
		{
			"name" : "samplerPosition",
			"binding" : 3,
			"offset" : -1,
			"size" : -1,
			"glType" : 35678,
			"readOnly" : false,
			"writeOnly" : false,
			"stageFlags" : 16
		},
		{
			"name" : "samplerDiffuse",
			"binding" : 4,
			"offset" : -1,
			"size" : -1,
			"glType" : 35678,
			"readOnly" : false,
			"writeOnly" : false,
			"stageFlags" : 16
		},
		{
			"name" : "samplerNormal",
			"binding" : 5,
			"offset" : -1,
			"size" : -1,
			"glType" : 35678,
			"readOnly" : false,
			"writeOnly" : false,
			"stageFlags" : 16
		},
		{
			"name" : "samplerMaterial",
			"binding" : 6,
			"offset" : -1,
			"size" : -1,
			"glType" : 35678,
			"readOnly" : false,
			"writeOnly" : false,
			"stageFlags" : 16
		},
		{
			"name" : "samplerBRDF",
			"binding" : 7,
			"offset" : -1,
			"size" : -1,
			"glType" : 35678,
			"readOnly" : false,
			"writeOnly" : false,
			"stageFlags" : 16
		},
		{
			"name" : "samplerIrradiance",
			"binding" : 8,
			"offset" : -1,
			"size" : -1,
			"glType" : 35680,
			"readOnly" : false,
			"writeOnly" : false,
			"stageFlags" : 16
		},
		{
			"name" : "samplerPrefiltered",
			"binding" : 9,
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
			"name" : "gl_Vertex",
			"set": 63,
			"location" : 4095,
			"size" : 4,
			"glType" : 5124
		}
	]
}