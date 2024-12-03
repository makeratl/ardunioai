<?php
// API Configuration
define('API_VERSION', '1.0.0');
define('DATA_PATH', __DIR__ . '/data/');

// Ensure data directory exists
if (!file_exists(DATA_PATH)) {
    mkdir(DATA_PATH, 0755, true);
} 