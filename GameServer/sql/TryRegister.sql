CREATE DEFINER=`root`@`localhost` PROCEDURE `TryRegister`(IN ID VARCHAR(100), IN PW VARCHAR(100))
BEGIN
    DECLARE userIndex INT DEFAULT NULL;
    DECLARE cnt INT DEFAULT 0;

    -- Check if user with given ID exists
    SELECT COUNT(*) INTO cnt
    FROM userinfo
    WHERE userinfo.id = ID;

    -- If no user exists, insert new record
    IF cnt = 0 THEN
        INSERT INTO userinfo(id, pw)
        VALUES (ID, PW);

        SELECT LAST_INSERT_ID() AS userIdx;
    ELSE
        -- If user exists, get userIdx and return -1
        SELECT userinfo.userIdx 
        INTO userIndex
        FROM userinfo
        WHERE userinfo.id = ID;

        SELECT -1 AS userIdx;
    END IF;
END