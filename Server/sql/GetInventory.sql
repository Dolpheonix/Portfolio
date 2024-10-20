CREATE DEFINER=`root`@`localhost` PROCEDURE `GetInventory`(IN userIndex INT)
BEGIN
	SELECT *
	FROM inventory
	WHERE userIdx = userIndex;
END