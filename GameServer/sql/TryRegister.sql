CREATE DEFINER=`root`@`localhost` PROCEDURE `TryRegister`(IN ID VARCHAR(100), IN PW VARCHAR(100))
BEGIN
    DECLARE userIndex INT DEFAULT NULL;
    DECLARE cnt INT DEFAULT 0;

    SELECT COUNT(*) INTO cnt
    FROM userinfo
    WHERE userinfo.id = ID;

    IF cnt = 0 THEN
        INSERT INTO userinfo(id, pw)
        VALUES (ID, PW);

        SELECT LAST_INSERT_ID() AS userIdx;
    ELSE
        SELECT userinfo.userIdx 
        INTO userIndex
        FROM userinfo
        WHERE userinfo.id = ID;

        SELECT -1 AS userIdx;
    END IF;
END