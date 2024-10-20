CREATE DEFINER=`root`@`localhost` PROCEDURE `GetPlayerInfo`(IN userIndex INT)
BEGIN
	SELECT * 
    FROM playerinfo
    WHERE userIdx = userIndex;
END