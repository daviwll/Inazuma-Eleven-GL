#include "match.hpp"
#include <GLFW/glfw3.h>
#include <cmath>
#include "hud.hpp"
#include "stadium.hpp"

static float clampf(float v, float lo, float hi)
{
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

static void resolveCircleRect(float& cx, float& cy, float radius, float rx0, float ry0, float rx1, float ry1, float& outNx, float& outNy, float& outPen)
{
    float closestX = clampf(cx, rx0, rx1);
    float closestY = clampf(cy, ry0, ry1);
    float dx = cx - closestX;
    float dy = cy - closestY;
    float dist2 = dx * dx + dy * dy;
    float r2 = radius * radius;
    if (dist2 > r2)
    {
        outPen = 0.0f;
        outNx = 0.0f;
        outNy = 0.0f;
        return;
    }

    float dist = std::sqrt(dist2);
    if (dist < 0.0001f)
    {
        outNx = 0.0f;
        outNy = 1.0f;
        outPen = radius;
        return;
    }
    outNx = dx / dist;
    outNy = dy / dist;
    outPen = radius - dist;
}


Match::Match()
    : players(createDefaultPlayers()),
      keepers(createDefaultGoalkeepers()),
      ball(0.0f, 0.0f, 0.02f),
      activePlayerIndex(0),
      ballPossessed(false),
      ballOwnerIndex(-1),
      aimX(1.0f),
      aimY(0.0f),
      passCharge(0.0f),
      shootCharge(0.0f),
      scoreLeft(0),
      scoreRight(0),
      prevSpace(GLFW_RELEASE),
      prevTab(GLFW_RELEASE),
      prevShoot(GLFW_RELEASE)
{
}

void Match::update(float dtSeconds, float timeSeconds, void* glfwWindow)
{
    GLFWwindow* window = static_cast<GLFWwindow*>(glfwWindow);

    if (players.empty()) return;

    Player& active = players[activePlayerIndex];

    int tabState = glfwGetKey(window, GLFW_KEY_TAB);
    if (tabState == GLFW_PRESS && prevTab == GLFW_RELEASE)
    {
        int bestIndex = activePlayerIndex;
        float bestDist = 1000.0f;
        for (size_t i = 0; i < players.size(); ++i)
        {
            if (players[i].teamId != active.teamId) continue;
            float dx = players[i].x - ball.x;
            float dy = players[i].y - ball.y;
            float d2 = dx * dx + dy * dy;
            if (d2 < bestDist)
            {
                bestDist = d2;
                bestIndex = static_cast<int>(i);
            }
        }
        activePlayerIndex = bestIndex;
    }
    prevTab = tabState;

    float moveX = 0.0f;
    float moveY = 0.0f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) moveY += 1.0f;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) moveY -= 1.0f;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) moveX -= 1.0f;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) moveX += 1.0f;

    float sprintMultiplier = 1.0f;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS && active.stamina > 0.0f)
    {
        sprintMultiplier = 1.5f;
        active.stamina -= dtSeconds * 0.6f;
        if (active.stamina < 0.0f) active.stamina = 0.0f;
    }
    else
    {
        active.stamina += dtSeconds * 0.3f;
        if (active.stamina > active.maxStamina) active.stamina = active.maxStamina;
    }

    if (moveX != 0.0f || moveY != 0.0f)
    {
        float len = std::sqrt(moveX * moveX + moveY * moveY);
        moveX /= len;
        moveY /= len;
        aimX = moveX;
        aimY = moveY;
        active.move(moveX * active.speed * sprintMultiplier * dtSeconds, moveY * active.speed * sprintMultiplier * dtSeconds);
        active.x = clampf(active.x, FIELD_LEFT + active.radius, FIELD_RIGHT - active.radius);
        active.y = clampf(active.y, FIELD_BOTTOM + active.radius, FIELD_TOP - active.radius);
        active.setBaseToCurrent();
    }

    // Possession / pickup
    if (!ballPossessed)
    {
        float dx = ball.x - active.x;
        float dy = ball.y - active.y;
        float dist2 = dx * dx + dy * dy;
        float pickup = active.radius + ball.radius + 0.01f;
        if (dist2 <= pickup * pickup)
        {
            ballPossessed = true;
            ballOwnerIndex = activePlayerIndex;
            ball.stop();
        }
    }

    if (!ballPossessed)
    {
        for (size_t i = 0; i < players.size(); ++i)
        {
            if (players[i].teamId == active.teamId) continue;
            float dx = ball.x - players[i].x;
            float dy = ball.y - players[i].y;
            float dist2 = dx * dx + dy * dy;
            float pickup = players[i].radius + ball.radius + 0.01f;
            if (dist2 <= pickup * pickup)
            {
                ballPossessed = true;
                ballOwnerIndex = static_cast<int>(i);
                ball.stop();
                break;
            }
        }
    }

    if (ballPossessed && ballOwnerIndex >= 0)
    {
        Player& owner = players[ballOwnerIndex];
        if (owner.teamId != active.teamId)
        {
            float dx = owner.x - active.x;
            float dy = owner.y - active.y;
            float dist2 = dx * dx + dy * dy;
            float tackleRange = owner.radius + active.radius + 0.01f;
            if (dist2 <= tackleRange * tackleRange)
            {
                ballOwnerIndex = activePlayerIndex;
            }
        }
    }

    // Pass
    int spaceState = glfwGetKey(window, GLFW_KEY_SPACE);
    if (spaceState == GLFW_PRESS && ballPossessed)
    {
        passCharge += dtSeconds;
        if (passCharge > 1.0f) passCharge = 1.0f;
    }

    if (spaceState == GLFW_RELEASE && prevSpace == GLFW_PRESS && ballPossessed)
    {
        int targetIndex = -1;
        float bestDist = 1000.0f;
        for (size_t i = 0; i < players.size(); ++i)
        {
            if (players[i].teamId != active.teamId) continue;
            if (static_cast<int>(i) == activePlayerIndex) continue;
            float dx = players[i].x - active.x;
            float dy = players[i].y - active.y;
            float d2 = dx * dx + dy * dy;
            if (d2 < bestDist)
            {
                bestDist = d2;
                targetIndex = static_cast<int>(i);
            }
        }

        float targetX = active.x + aimX * 0.45f;
        float targetY = active.y + aimY * 0.45f;
        if (targetIndex >= 0)
        {
            targetX = players[targetIndex].x;
            targetY = players[targetIndex].y;
            activePlayerIndex = targetIndex;
        }

        float kickSpeed = 1.4f + passCharge * 3.0f;
        passCharge = 0.0f;

        ballPossessed = false;
        ballOwnerIndex = -1;
        ball.kickTo(targetX, targetY, kickSpeed);
    }
    prevSpace = spaceState;

    int shootState = glfwGetKey(window, GLFW_KEY_E);
    if (shootState == GLFW_PRESS && ballPossessed)
    {
        shootCharge += dtSeconds;
        if (shootCharge > 1.0f) shootCharge = 1.0f;
    }

    if (shootState == GLFW_RELEASE && prevShoot == GLFW_PRESS && ballPossessed)
    {
        float targetX = active.x + aimX * 1.4f;
        float targetY = active.y + aimY * 1.4f;
        float shootSpeed = 3.0f + shootCharge * 6.0f;
        shootCharge = 0.0f;

        ballPossessed = false;
        ballOwnerIndex = -1;
        ball.kickTo(targetX, targetY, shootSpeed);
    }

    if (shootState == GLFW_RELEASE && !ballPossessed)
    {
        shootCharge = 0.0f;
    }
    prevShoot = shootState;

    if (ballPossessed)
    {
        Player& owner = players[ballOwnerIndex >= 0 ? ballOwnerIndex : activePlayerIndex];
        ball.setPosition(owner.x + owner.radius + ball.radius + 0.01f, owner.y);
    }
    else
    {
        ball.update(dtSeconds);
    }

    for (Goalkeeper& keeper : keepers)
    {
        float rx0 = keeper.x - keeper.halfW;
        float rx1 = keeper.x + keeper.halfW;
        float ry0 = keeper.y - keeper.halfH;
        float ry1 = keeper.y + keeper.halfH;

        float nx = 0.0f;
        float ny = 0.0f;
        float pen = 0.0f;
        resolveCircleRect(ball.x, ball.y, ball.radius, rx0, ry0, rx1, ry1, nx, ny, pen);
        if (pen > 0.0f)
        {
            ball.x += nx * pen;
            ball.y += ny * pen;
            float dot = ball.vx * nx + ball.vy * ny;
            if (dot < 0.0f)
            {
                ball.vx -= 2.0f * dot * nx;
                ball.vy -= 2.0f * dot * ny;
                ball.vx *= 0.6f;
                ball.vy *= 0.6f;
            }
        }
    }

    // Stadium boundaries for ball
    if (ball.y - ball.radius < FIELD_BOTTOM)
    {
        ball.y = FIELD_BOTTOM + ball.radius;
        ball.vy = -ball.vy * 0.6f;
    }
    else if (ball.y + ball.radius > FIELD_TOP)
    {
        ball.y = FIELD_TOP - ball.radius;
        ball.vy = -ball.vy * 0.6f;
    }

    if (std::fabs(ball.y) > GOAL_HALF_HEIGHT)
    {
        if (ball.x - ball.radius < FIELD_LEFT)
        {
            ball.x = FIELD_LEFT + ball.radius;
            ball.vx = -ball.vx * 0.6f;
        }
        else if (ball.x + ball.radius > FIELD_RIGHT)
        {
            ball.x = FIELD_RIGHT - ball.radius;
            ball.vx = -ball.vx * 0.6f;
        }
    }

    if (ball.x > FIELD_RIGHT && std::fabs(ball.y) < GOAL_HALF_HEIGHT)
    {
        scoreLeft += 1;
        ball.setPosition(0.0f, 0.0f);
        ball.stop();
        ballPossessed = false;
        ballOwnerIndex = -1;
    }
    else if (ball.x < FIELD_LEFT && std::fabs(ball.y) < GOAL_HALF_HEIGHT)
    {
        scoreRight += 1;
        ball.setPosition(0.0f, 0.0f);
        ball.stop();
        ballPossessed = false;
        ballOwnerIndex = -1;
    }

    // Goalkeeper AI: move along goal line to track ball
    for (Goalkeeper& keeper : keepers)
    {
        float targetY = clampf(ball.y, -0.5f, 0.5f);
        float dy = targetY - keeper.y;
        float step = keeper.speed * dtSeconds;
        if (std::fabs(dy) > step)
        {
            keeper.y += (dy > 0.0f ? step : -step);
        }
        else
        {
            keeper.y = targetY;
        }
        keeper.baseY = keeper.y;
    }

    for (Player& player : players)
    {
        if (static_cast<int>(&player - &players[0]) == activePlayerIndex) continue;
        float targetX = player.baseX;
        float targetY = player.baseY;
        if (player.teamId == 0)
        {
            targetX = (ball.x < 0.0f) ? player.baseX + 0.1f : player.baseX;
        }
        else
        {
            targetX = (ball.x > 0.0f) ? player.baseX - 0.1f : player.baseX;
        }
        player.moveToward(targetX, targetY, player.speed * 0.6f, dtSeconds);
        player.x = clampf(player.x, FIELD_LEFT + player.radius, FIELD_RIGHT - player.radius);
        player.y = clampf(player.y, FIELD_BOTTOM + player.radius, FIELD_TOP - player.radius);
        player.setBaseToCurrent();
    }

    for (Player& player : players)
    {
        for (Goalkeeper& keeper : keepers)
        {
            float rx0 = keeper.x - keeper.halfW;
            float rx1 = keeper.x + keeper.halfW;
            float ry0 = keeper.y - keeper.halfH;
            float ry1 = keeper.y + keeper.halfH;

            float nx = 0.0f;
            float ny = 0.0f;
            float pen = 0.0f;
            resolveCircleRect(player.x, player.y, player.radius, rx0, ry0, rx1, ry1, nx, ny, pen);
            if (pen > 0.0f)
            {
                player.x += nx * pen;
                player.y += ny * pen;
                player.setBaseToCurrent();
            }
        }
    }


}

void Match::render()
{
    for (Goalkeeper& keeper : keepers)
    {
        keeper.render();
    }

    for (Player& player : players)
    {
        player.render();
    }

    ball.render();
}

void Match::renderHud()
{
    drawHud(scoreLeft, scoreRight, passCharge);
}
