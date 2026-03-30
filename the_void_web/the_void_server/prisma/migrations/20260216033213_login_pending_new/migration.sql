/*
  Warnings:

  - You are about to drop the column `token` on the `void_login_pending` table. All the data in the column will be lost.

*/
-- AlterTable
ALTER TABLE `void_login_pending` DROP COLUMN `token`;
