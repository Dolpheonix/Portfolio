CREATE DEFINER=`root`@`localhost` PROCEDURE `SaveInventory`(IN p_userIdx INT, IN p_itemType ENUM('CLOTH', 'WEAPON', 'ITEM'), IN p_inventoryIdx INT, 
								  IN p_infoIndex BIGINT, IN p_num BIGINT)
BEGIN
	IF EXISTS (SELECT 1 FROM inventory WHERE userIdx = p_userIdx AND itemType = p_itemType AND inventoryIdx = p_inventoryIdx) THEN
		UPDATE inventory
        SET
			infoIndex = p_infoIndex,
            num = p_num
		WHERE userIdx = p_userIdx AND itemType = p_itemType AND inventoryIdx = p_inventoryIdx;
	ELSE
		INSERT INTO inventory (userIdx, itemType, inventoryIdx, infoIndex, num)
		VALUES (p_userIdx, p_itemType, p_inventoryIdx, p_infoIndex, p_num);
    END IF;
END