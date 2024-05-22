CREATE DEFINER=`root`@`localhost` PROCEDURE `TryLogin`(IN ID VARCHAR(100), IN PW VARCHAR(100))
BEGIN
	DECLARE userIndex INT;
    SET userIndex = NULL;
    
	SELECT userinfo.userIdx
		INTO userIndex
        FROM userInfo
		WHERE (id = ID) AND (pw = PW);
        
	IF userIndex IS NULL THEN
		SET userIndex = -1;
	END IF;
    
    SELECT userIndex AS userIdx;
    
    IF userIdx != -1 THEN
		SELECT *
        FROM playerinfo
        WHERE userIdx = userIndex;
        
        SELECT *
        FROM inventory
        WHERE userIdx = userIndex;
        
        SELECT *
        FROM queststatus
        WHERE userIdx = userIndex;
	END IF;
END