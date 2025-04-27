CREATE DATABASE IF NOT EXISTS chronix CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;

USE chronix;

CREATE DATABASE IF NOT EXISTS chronix;
USE chronix;

CREATE TABLE IF NOT EXISTS `jobs` (
  `id` INT NOT NULL AUTO_INCREMENT COMMENT '任务ID',
  `type` ENUM('ONCE', 'CRON') NOT NULL COMMENT '任务类型 CRON-周期任务 ONCE-一次性任务',
  `expr` VARCHAR(255) COLLATE utf8mb4_unicode_ci NOT NULL COMMENT '周期任务Cron表达式',
  `status` VARCHAR(20) COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT 'Pending' COMMENT '任务状态 Pending-排队中, Running-执行中 Paused-暂停中',
  `result` VARCHAR(20) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT 'Unknown' COMMENT '任务最近结果 Unknown-未知 Success-成功 Failed-失败',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci COMMENT='任务表';
