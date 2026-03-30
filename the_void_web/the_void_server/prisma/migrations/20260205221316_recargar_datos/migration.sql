-- CreateTable
CREATE TABLE `void_products` (
    `id` VARCHAR(191) NOT NULL,
    `name` VARCHAR(250) NOT NULL,
    `price` DOUBLE NOT NULL,

    PRIMARY KEY (`id`)
) DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;

-- CreateTable
CREATE TABLE `void_product_modules` (
    `id` VARCHAR(191) NOT NULL,
    `module` VARCHAR(191) NOT NULL,
    `active` BOOLEAN NOT NULL DEFAULT true,

    PRIMARY KEY (`id`)
) DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;

-- CreateTable
CREATE TABLE `void_product_pivot` (
    `product_id` VARCHAR(191) NOT NULL,
    `module_id` VARCHAR(191) NOT NULL,

    PRIMARY KEY (`product_id`, `module_id`)
) DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;

-- AddForeignKey
ALTER TABLE `void_product_pivot` ADD CONSTRAINT `void_product_pivot_product_id_fkey` FOREIGN KEY (`product_id`) REFERENCES `void_products`(`id`) ON DELETE CASCADE ON UPDATE CASCADE;

-- AddForeignKey
ALTER TABLE `void_product_pivot` ADD CONSTRAINT `void_product_pivot_module_id_fkey` FOREIGN KEY (`module_id`) REFERENCES `void_product_modules`(`id`) ON DELETE CASCADE ON UPDATE CASCADE;
