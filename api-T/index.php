<?php
header('Content-Type: application/json');
header('Access-Control-Allow-Origin: *');

require_once 'config.php';

// Basic routing
$path = trim($_SERVER['REQUEST_URI'], '/');
$path = str_replace('api-T/', '', $path);

// Route the request
switch($path) {
    case '':
    case 'status':
        require_once 'endpoints/status.php';
        break;
        
    case 'matrix/config':
        require_once 'endpoints/matrix/config.php';
        break;
        
    case 'matrix/modes':
        require_once 'endpoints/matrix/modes.php';
        break;
        
    default:
        http_response_code(404);
        echo json_encode([
            'error' => 'Endpoint not found',
            'path' => $path
        ]);
} 