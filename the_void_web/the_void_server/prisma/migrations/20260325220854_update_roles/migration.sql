-- AlterTable
ALTER TABLE `void_users` MODIFY `role` ENUM('VOID', 'USER', 'BETA', 'ADMIN') NOT NULL DEFAULT 'USER';
