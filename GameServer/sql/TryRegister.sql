CREATE DEFINER=`root`@`localhost` PROCEDURE `TryRegister`(IN ID VARCHAR(100), IN PW VARCHAR(100))
BEGIN
	DECLARE userIndex INT;
    SET userIndex = NULL;
    
	SELECT userinfo.userIdx 
    INTO userIndex
    FROM userinfo
    WHERE id = ID;
    
    IF userIndex = NULL THEN
		INSERT INTO userinfo(id, pw)
        VALUES (ID, PW);
        
        SELECT LAST_INSERT_ID() AS userIdx;
	ELSE
		SELECT -1 AS userIdx;
	END IF;
END