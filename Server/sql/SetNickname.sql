CREATE DEFINER=`root`@`localhost` PROCEDURE `SetNickname`(IN p_userIdx INT, IN p_nickname VARCHAR(100))
BEGIN
	DECLARE nicknameExists INT DEFAULT 0;
    DECLARE userExists INT DEFAULT 0;

    SELECT COUNT(*) INTO nicknameExists
    FROM playerinfo
    WHERE nickname = p_nickname;

    SELECT COUNT(*) INTO userExists
    FROM playerinfo
    WHERE userIdx = p_userIdx;

    IF nicknameExists > 0 THEN
        SELECT false AS Succeeded;
    ELSEIF userExists = 0 THEN
        SELECT false AS Succeeded;
    ELSE
        UPDATE PlayerInfo
        SET nickname = p_nickname
        WHERE userIdx = p_userIdx;
        
        SELECT true AS Succeeded;
    END IF;
END