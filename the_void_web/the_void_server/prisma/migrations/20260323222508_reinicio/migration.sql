/*
  Warnings:

  - You are about to drop the column `user_id` on the `void_module_config` table. All the data in the column will be lost.
  - You are about to alter the column `module` on the `void_module_config` table. The data in that column could be lost. The data in that column will be cast from `VarChar(191)` to `VarChar(50)`.
  - You are about to alter the column `action` on the `void_module_config` table. The data in that column could be lost. The data in that column will be cast from `VarChar(191)` to `VarChar(20)`.
  - You are about to alter the column `module` on the `void_product_modules` table. The data in that column could be lost. The data in that column will be cast from `VarChar(191)` to `VarChar(50)`.
  - A unique constraint covering the columns `[profile_id,module]` on the table `void_module_config` will be added. If there are existing duplicate values, this will fail.
  - A unique constraint covering the columns `[module]` on the table `void_product_modules` will be added. If there are existing duplicate values, this will fail.
  - A unique constraint covering the columns `[username]` on the table `void_users` will be added. If there are existing duplicate values, this will fail.
  - Added the required column `profile_id` to the `void_module_config` table without a default value. This is not possible if the table is not empty.
  - Added the required column `duration_days` to the `void_products` table without a default value. This is not possible if the table is not empty.

*/
-- DropForeignKey
ALTER TABLE `void_module_config` DROP FOREIGN KEY `void_module_config_user_id_fkey`;

-- DropIndex
DROP INDEX `void_module_config_user_id_module_key` ON `void_module_config`;

-- AlterTable
ALTER TABLE `void_module_config` DROP COLUMN `user_id`,
    ADD COLUMN `profile_id` VARCHAR(191) NOT NULL,
    MODIFY `module` VARCHAR(50) NOT NULL,
    MODIFY `action` VARCHAR(20) NOT NULL;

-- AlterTable
ALTER TABLE `void_product_modules` MODIFY `module` VARCHAR(50) NOT NULL;

-- AlterTable
ALTER TABLE `void_products` ADD COLUMN `duration_days` INTEGER NOT NULL;

-- AlterTable
ALTER TABLE `void_users` ADD COLUMN `ban_reason` VARCHAR(255) NULL,
    ADD COLUMN `last_hwid_reset` DATETIME(3) NULL,
    ADD COLUMN `last_ip` VARCHAR(45) NULL,
    ADD COLUMN `last_login` DATETIME(3) NULL,
    ADD COLUMN `role` ENUM('USER', 'BETA', 'ADMIN') NOT NULL DEFAULT 'USER';

-- CreateTable
CREATE TABLE `void_profiles` (
    `id` VARCHAR(191) NOT NULL,
    `user_id` VARCHAR(191) NOT NULL,
    `name` VARCHAR(50) NOT NULL,
    `is_default` BOOLEAN NOT NULL DEFAULT false,

    UNIQUE INDEX `void_profiles_user_id_name_key`(`user_id`, `name`),
    PRIMARY KEY (`id`)
) DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;

-- CreateTable
CREATE TABLE `void_subscriptions` (
    `id` VARCHAR(191) NOT NULL,
    `user_id` VARCHAR(191) NOT NULL,
    `product_id` VARCHAR(191) NOT NULL,
    `active` BOOLEAN NOT NULL DEFAULT true,
    `starts_at` DATETIME(3) NOT NULL DEFAULT CURRENT_TIMESTAMP(3),
    `expires_at` DATETIME(3) NOT NULL,

    INDEX `void_subscriptions_user_id_idx`(`user_id`),
    PRIMARY KEY (`id`)
) DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;

-- CreateIndex
CREATE UNIQUE INDEX `void_module_config_profile_id_module_key` ON `void_module_config`(`profile_id`, `module`);

-- CreateIndex
CREATE UNIQUE INDEX `void_product_modules_module_key` ON `void_product_modules`(`module`);

-- CreateIndex
CREATE UNIQUE INDEX `void_users_username_key` ON `void_users`(`username`);

-- AddForeignKey
ALTER TABLE `void_profiles` ADD CONSTRAINT `void_profiles_user_id_fkey` FOREIGN KEY (`user_id`) REFERENCES `void_users`(`id`) ON DELETE CASCADE ON UPDATE CASCADE;

-- AddForeignKey
ALTER TABLE `void_module_config` ADD CONSTRAINT `void_module_config_profile_id_fkey` FOREIGN KEY (`profile_id`) REFERENCES `void_profiles`(`id`) ON DELETE CASCADE ON UPDATE CASCADE;

-- AddForeignKey
ALTER TABLE `void_subscriptions` ADD CONSTRAINT `void_subscriptions_user_id_fkey` FOREIGN KEY (`user_id`) REFERENCES `void_users`(`id`) ON DELETE CASCADE ON UPDATE CASCADE;

-- AddForeignKey
ALTER TABLE `void_subscriptions` ADD CONSTRAINT `void_subscriptions_product_id_fkey` FOREIGN KEY (`product_id`) REFERENCES `void_products`(`id`) ON DELETE CASCADE ON UPDATE CASCADE;
