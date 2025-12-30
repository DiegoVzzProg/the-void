-- CreateTable
CREATE TABLE `void_users` (
    `id` VARCHAR(191) NOT NULL,
    `email` VARCHAR(250) NOT NULL,
    `username` VARCHAR(50) NOT NULL,
    `password` VARCHAR(255) NOT NULL,
    `hwid` VARCHAR(250) NULL,
    `banned` BOOLEAN NOT NULL DEFAULT false,

    UNIQUE INDEX `void_users_email_key`(`email`),
    UNIQUE INDEX `void_users_hwid_key`(`hwid`),
    PRIMARY KEY (`id`)
) DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;
