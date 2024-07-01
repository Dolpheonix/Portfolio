CREATE DEFINER=`root`@`localhost` PROCEDURE `TryLogin`(IN ID VARCHAR(100), IN PW VARCHAR(100))
BEGIN
	DECLARE userIndex INT DEFAULT NULL;

    SELECT userIdx INTO userIndex
    FROM userinfo
    WHERE userinfo.id = ID AND userinfo.pw = PW;
    
    IF userIndex IS NULL THEN
		SELECT -1 AS userIdx; 
	ELSE
		SELECT userIndex AS userIdx;
	END IF;
END