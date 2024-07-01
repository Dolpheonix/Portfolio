CREATE DEFINER=`root`@`localhost` PROCEDURE `SavePlayerInfo`(IN p_userIdx INT, IN p_name VARCHAR(100), IN p_level INT, IN p_map INT,
															 IN p_loc_x FLOAT, IN p_loc_y FLOAT, IN p_loc_z FLOAT, IN p_gold BIGINT)
BEGIN
	IF EXISTS (SELECT 1 FROM playerinfo WHERE userIdx = p_userIdx) THEN
		UPDATE playerinfo
        SET nickname = p_name,
			playerLevel = p_level,
            mapIdx = p_map,
            loc_x = p_loc_x,
            loc_y = p_loc_y,
            loc_z = p_loc_z,
            gold = p_gold
		WHERE userIdx = p_userIdx;
	ELSE
		INSERT INTO playerinfo(userIdx, nickname, playerLevel, mapIdx, loc_x, loc_y, loc_z, gold)
        VALUES (p_userIdx, p_name, p_level, p_map, p_loc_x, p_loc_y, p_loc_z, p_gold);
	END IF;
END