<?php
echo json_encode([
    'modes' => [
        [
            'id' => 1,
            'name' => 'pulse',
            'description' => 'Pulsing rainbow colors'
        ],
        [
            'id' => 2,
            'name' => 'spiral',
            'description' => 'Spiral pattern animation'
        ],
        [
            'id' => 3,
            'name' => 'fire',
            'description' => 'Fire effect animation'
        ],
        [
            'id' => 4,
            'name' => 'wave',
            'description' => 'Wave spectrum animation'
        ],
        [
            'id' => 5,
            'name' => 'cloudmist',
            'description' => 'Cloud mist effect'
        ]
    ]
]); 