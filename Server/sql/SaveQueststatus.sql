CREATE DEFINER=`root`@`localhost` PROCEDURE `SaveQueststatus`(IN p_userIdx INT, IN p_questIdx INT, 
									IN p_progress ENUM('Unavailable','Available','InProgress','Completable','Completed'),
                                    IN p_subquestIdx INT, IN p_currPhase INT, IN p_completed INT, 
                                    IN p_bStarted BOOLEAN, IN p_bCompleted BOOLEAN, IN p_currAmount INT)
BEGIN
	IF EXISTS (SELECT 1 FROM queststatus WHERE userIdx = p_userIdx AND questIdx = p_questIdx AND subquestIdx = p_subquestIdx) THEN
		UPDATE queststatus
		SET
			progress = p_progress,
			currPhase = p_currPhase,
			completed = p_completed,
			bStarted = p_bStarted,
			bCompleted = p_bCompleted,
			currAmount = p_currAmount
		WHERE userIdx = p_userIdx AND questIdx = p_questIdx AND subquestIdx = p_subquestIdx;
	ELSE
		INSERT INTO queststatus (userIdx, questIdx, progress, subquestIdx, currPhase, completed, bStarted, bCompleted, currAmount)
        VALUES (p_userIdx, p_questIdx, p_progress, p_subquestIdx, p_currPhase, p_completed, p_bStarted, p_bCompleted, p_currAmount);
    END IF;
END