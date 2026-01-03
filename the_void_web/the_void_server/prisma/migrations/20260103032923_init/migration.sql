/*
  Warnings:

  - You are about to alter the column `username` on the `void_users` table. The data in that column could be lost. The data in that column will be cast from `VarChar(50)` to `VarChar(16)`.

*/
-- AlterTable
ALTER TABLE `void_users` MODIFY `username` VARCHAR(16) NOT NULL;
