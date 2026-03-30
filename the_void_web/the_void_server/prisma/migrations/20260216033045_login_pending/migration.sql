-- CreateTable
CREATE TABLE `void_login_pending` (
    `id` VARCHAR(191) NOT NULL,
    `token` VARCHAR(36) NOT NULL,
    `temp_hwid` VARCHAR(255) NOT NULL,
    `created_at` DATETIME(3) NOT NULL DEFAULT CURRENT_TIMESTAMP(3),
    `expires_at` DATETIME(3) NOT NULL,

    PRIMARY KEY (`id`)
) DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;
