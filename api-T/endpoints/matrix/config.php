<?php
$statePath = DATA_PATH . 'state.json';

// Handle GET request
if ($_SERVER['REQUEST_METHOD'] === 'GET') {
    if (file_exists($statePath)) {
        echo file_get_contents($statePath);
    } else {
        // Default configuration
        $defaultState = [
            'mode' => 2,
            'brightness' => 128,
            'speed' => 50,
            'lastUpdate' => time()
        ];
        file_put_contents($statePath, json_encode($defaultState));
        echo json_encode($defaultState);
    }
}

// Handle POST request to update configuration
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $input = json_decode(file_get_contents('php://input'), true);
    
    if ($input) {
        $newState = [
            'mode' => isset($input['mode']) ? (int)$input['mode'] : 1,
            'brightness' => isset($input['brightness']) ? (int)$input['brightness'] : 128,
            'speed' => isset($input['speed']) ? (int)$input['speed'] : 50,
            'lastUpdate' => time()
        ];
        
        file_put_contents($statePath, json_encode($newState));
        echo json_encode($newState);
    } else {
        http_response_code(400);
        echo json_encode(['error' => 'Invalid input']);
    }
} 