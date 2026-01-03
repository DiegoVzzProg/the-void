-- CreateTable
CREATE TABLE `void_module_config` (
    `id` VARCHAR(191) NOT NULL,
    `user_id` VARCHAR(191) NOT NULL,
    `module` VARCHAR(191) NOT NULL,
    `action` VARCHAR(191) NOT NULL,
    `settings` JSON NOT NULL,
    `conditions` JSON NOT NULL,

    UNIQUE INDEX `void_module_config_user_id_module_key`(`user_id`, `module`),
    PRIMARY KEY (`id`)
) DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;

-- AddForeignKey
ALTER TABLE `void_module_config` ADD CONSTRAINT `void_module_config_user_id_fkey` FOREIGN KEY (`user_id`) REFERENCES `void_users`(`id`) ON DELETE CASCADE ON UPDATE CASCADE;
