CREATE DATABASE IF NOT EXISTS chronix CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;

USE chronix;

CREATE DATABASE IF NOT EXISTS chronix;
USE chronix;

CREATE TABLE `jobs` (
  `id` int NOT NULL,
  `expr` varchar(255) COLLATE utf8mb4_unicode_ci NOT NULL,
  `status` varchar(20) COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT 'Pending',
  `result` varchar(20) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT 'Unknown',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
