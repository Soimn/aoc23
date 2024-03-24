#define SMN_IMPLEMENTATION
#include "../smn.h"

enum Direction
{
  Dir_N = 0,
  //Dir_NE,
  Dir_E,
  //Dir_SE,
  Dir_S,
  //Dir_SW,
  Dir_W,
  //Dir_NW,
  DIRECTION_COUNT
};

#define OPPOSITE_DIR(D) (((D) + DIRECTION_COUNT/2) % DIRECTION_COUNT)
#define X_STEP(D) (((D) == Dir_E) - ((D) == Dir_W))
#define Y_STEP(D) (((D) == Dir_N) - ((D) == Dir_S))

typedef struct Node_Pred
{
  u32 dist;
  u32 prev_node;
  u8 pred_idx;
  u8 dir;
  u8 ttl;
} Node_Pred;

typedef struct Node
{
  s8 path_dir;
  u8 weight;
  Node_Pred preds[4*10];
} Node;

typedef struct Edge
{
  u32 key;
  u32 start_node;
  u32 end_node;
  u8 pred_idx;
  u8 dir;
  u8 ttl;
} Edge;

typedef struct Edge_Queue
{
  SB(Edge) backing;
  u32 heap_size;
} Edge_Queue;

bool
EdgeQueue_IsEmpty(Edge_Queue* queue)
{
  return (queue->heap_size == 0);
}

void
EdgeQueue_Enqueue(Edge_Queue* queue, Edge edge)
{
  SB_Append(&queue->backing, edge);
  queue->heap_size += 1;

  uint idx = queue->heap_size-1;

  uint parent_idx = idx/2;
  while (queue->backing[idx].key < queue->backing[parent_idx].key)
  {
    Edge tmp = queue->backing[idx];
    queue->backing[idx]        = queue->backing[parent_idx];
    queue->backing[parent_idx] = tmp;

    idx        = parent_idx;
    parent_idx = idx/2;
  }
}

Edge
EdgeQueue_Dequeue(Edge_Queue* queue)
{
  ASSERT(queue->heap_size > 0);

  Edge dequeued_entry = queue->backing[0];

  queue->backing[0] = queue->backing[SB_Len(&queue->backing)-1];
  SB_RemoveLast(&queue->backing);
  queue->heap_size -= 1;

  for (uint smallest_idx = 0;;)
  {
    uint idx       = smallest_idx;
    uint left_idx  = 2*smallest_idx;
    uint right_idx = 2*smallest_idx + 1;

    if (left_idx < queue->heap_size && queue->backing[left_idx].key < queue->backing[smallest_idx].key)
    {
      smallest_idx = left_idx;
    }

    if (right_idx < queue->heap_size && queue->backing[right_idx].key < queue->backing[smallest_idx].key)
    {
      smallest_idx = right_idx;
    }

    if (smallest_idx == idx) break;
    else
    {
      Edge tmp = queue->backing[idx];
      queue->backing[idx]          = queue->backing[smallest_idx];
      queue->backing[smallest_idx] = tmp;
      continue;
    }
  }

  return dequeued_entry;
}

u32
Solve(String input, u8 min_step, u8 max_step, bool should_print)
{
  uint width = 0;
  while (width < input.size && input.data[width] != '\r') ++width;
  uint height = input.size / (width+2);

  Node* nodes = malloc(width*height*sizeof(Node));

  for (uint j = 0; j < height; ++j)
  {
    for (uint i = 0; i < width; ++i)
    {
      nodes[j*width + i] = (Node){
        .path_dir = -1,
        .weight   = input.data[j*(width+2) + i]&0xF,
      };

      for (uint k = 0; k < ARRAY_SIZE(nodes[0].preds); ++k) nodes[j*width + i].preds[k].dist = U32_MAX;
    }
  }

  Edge_Queue queue = {0};

  EdgeQueue_Enqueue(&queue, (Edge){
      .key        = nodes[1].weight,
      .start_node = 0,
      .end_node   = 1,
      .dir        = Dir_E,
      .ttl        = max_step-1
  });
  EdgeQueue_Enqueue(&queue, (Edge){
      .key        = nodes[width].weight,
      .start_node = 0,
      .end_node   = width,
      .dir        = Dir_N,
      .ttl        = max_step-1
  });

  u32 goal_node = width*height-1;
  s8 goal_ttl   = -1;
  while (goal_ttl == -1)
  {
    Edge edge = EdgeQueue_Dequeue(&queue);

    if (nodes[edge.end_node].preds[edge.dir*10 + edge.ttl].dist == U32_MAX)
    {
      u8 insert_idx = edge.dir*10 + edge.ttl;

      nodes[edge.end_node].preds[insert_idx] = (Node_Pred){
        .dist      = edge.key,
        .prev_node = edge.start_node,
        .pred_idx  = edge.pred_idx,
        .dir       = edge.dir,
        .ttl       = edge.ttl,
      };

      for (u8 i = 0; i < DIRECTION_COUNT; ++i)
      {
        s8 dx  = X_STEP(i);
        s8 dy  = Y_STEP(i);
        s8 ttl = (s8)(i == edge.dir ? edge.ttl-1 : max_step-1);

        smm dx_rest_step = dx*(i == edge.dir ? 1 : min_step);
        smm dy_rest_step = dy*(i == edge.dir ? 1 : min_step);
        if (ttl >= 0                         &&
            (i == edge.dir || edge.ttl <= 6) &&
            i != OPPOSITE_DIR(edge.dir))
        {
          if (!((smm)(edge.end_node % width) >= -dx_rest_step          &&
             (smm)(edge.end_node % width) < (smm)width - dx_rest_step &&
             (smm)(edge.end_node / width) >= -dy_rest_step            &&
             (smm)(edge.end_node / width) < (smm)height - dy_rest_step))
          {
            volatile int a = 0;
          }
          else
          {
          u32 end_node = edge.end_node + dy*width + dx;
          u32 dist     = edge.key + nodes[end_node].weight;
          
          if (end_node == goal_node)
          {
            nodes[end_node].preds[ttl] = (Node_Pred){
              .dist      = dist,
              .prev_node = edge.end_node,
              .pred_idx  = insert_idx,
              .dir       = i,
              .ttl       = ttl,
            };

            goal_ttl = ttl;

            break;
          }
          else if (nodes[end_node].preds[i*10 + ttl].dist == U32_MAX)
          {
            EdgeQueue_Enqueue(&queue, (Edge){
                .key        = dist,
                .start_node = edge.end_node,
                .end_node   = end_node,
                .pred_idx   = insert_idx,
                .dir        = i,
                .ttl        = ttl,
            });
          }
          }
        }
      }
    }
  }

  for (u32 node = goal_node, pred = goal_ttl; node != 0;)
  {
    nodes[node].path_dir = nodes[node].preds[pred].dir;

    u32 prev_node = nodes[node].preds[pred].prev_node;
    u32 prev_pred = nodes[node].preds[pred].pred_idx;

    node = prev_node;
    pred = prev_pred;
  }

  if (should_print)
  {
    for (uint j = 0; j < height; ++j)
    {
      for (uint i = 0; i < width; ++i)
      {
        Node* node = &nodes[j*width + i];
        putchar(node->path_dir == -1 ? '0'+node->weight : (u8[4]){'v', '>', '^', '<'}[node->path_dir]);
      }
      putchar('\n');
    }
  }

  u32 result = nodes[goal_node].preds[goal_ttl].dist;

  free(nodes);
  SB_Free(&queue.backing);

 return result;
}

int
main(int argc, char** argv)
{
  ASSERT(argc == 2);

  String input = {0};
  {
    FILE* file;
    errno_t open_err = fopen_s(&file, argv[1], "rb");
    ASSERT(open_err == 0);

    fseek(file, 0, SEEK_END);
    umm file_size = ftell(file);
    rewind(file);

    input.data = malloc(file_size);
    input.size = file_size;

    umm bytes_read = fread(input.data, 1, file_size, file);

    ASSERT(bytes_read == file_size);

    fclose(file);
  }
  u32 part1_result = Solve(input, 1, 3, false);
  u32 part2_result = Solve(input, 4, 10, true);
  printf("Part 1: %u\n", part1_result);
  printf("Part 2: %u\n", part2_result);

  return 0;
}
