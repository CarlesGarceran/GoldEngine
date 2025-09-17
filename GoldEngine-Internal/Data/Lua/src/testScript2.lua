local moveSpeed = 0.5;
local smooth = 1.25;

function Update()
	if(Input:IsKeyDown(65)) then
		script.GetTransform().parent.position.x = script.GetTransform().parent.position.x + (moveSpeed * smooth);
	end
end