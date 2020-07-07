{
    "attributes": [
        {
            "glType": 35665,
            "location": 0,
            "name": "inPosition",
            "set": 63,
            "size": 12
        },
        {
            "glType": 35664,
            "location": 1,
            "name": "inUV",
            "set": 63,
            "size": 8
        },
        {
            "glType": 35676,
            "location": 3,
            "name": "inModelMatrix",
            "set": 63,
            "size": 64
        },
        {
            "glType": 35666,
            "location": 7,
            "name": "inColorOffset",
            "set": 63,
            "size": 16
        },
        {
            "glType": 35666,
            "location": 8,
            "name": "inOffsets",
            "set": 63,
            "size": 16
        },
        {
            "glType": 35665,
            "location": 9,
            "name": "inBlend",
            "set": 63,
            "size": 12
        }
    ],
    "frag": "particle.frag",
    "uniforms": [
        {
            "binding": 1,
            "glType": 35678,
            "name": "samplerColor",
            "offset": 0,
            "readOnly": false,
            "size": -1,
            "stageFlags": 16,
            "writeOnly": false
        },
		{
            "binding": 2,
            "glType": 35678,
            "name": "samplerDepth",
            "offset": 0,
            "readOnly": false,
            "size": -1,
            "stageFlags": 16,
            "writeOnly": false
        }
    ],
    "uniforms_blocks": [
        {
            "name": "UniformScene",
            "size": 128,
            "stageFlags": 1,
			"binding" : 0,
            "uniforms": [
                {
                    "binding": -1,
                    "glType": 35676,
                    "name": "projection",
                    "offset": 0,
                    "readOnly": false,
                    "size": 64,
                    "stageFlags": 1,
                    "type": 0,
                    "writeOnly": false
                },
                {
                    "binding": -1,
                    "glType": 35676,
                    "name": "view",
                    "offset": 64,
                    "readOnly": false,
                    "size": 64,
                    "stageFlags": 1,
                    "type": 0,
                    "writeOnly": false
                }
            ]
        }
    ],
    "vert": "particle.vert"
}
