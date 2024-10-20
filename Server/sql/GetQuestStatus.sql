CREATE DEFINER=`root`@`localhost` PROCEDURE `GetQuestStatus`(IN userIndex INT)
BEGIN
	SELECT *
	FROM queststatus
	WHERE userIdx = userIndex;
END